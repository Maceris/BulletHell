#include "EventManager.h"

#include "Logger.h"

EventManager::EventManager()
	: handlers{}
	, main_queue{}
	, threadsafe_queue{}
{}

bool EventManager::register_handler(const EventHandler& handler,
	const EventType& type)
{
	//NOTE(ches) this will emplace an entry if it does not exist already
	HandlerList& handler_list = handlers[type];

	for (auto i = handler_list.begin(); i < handler_list.end(); ++i)
	{
		if (handler == (*i))
		{
			LOG_WARNING("Attempting to register a handler twice");
			return false;
		}
	}
	handler_list.push_back(handler);
	LOG_TAGGED("Event", "Registered event handler for event type " 
		+ std::to_string(type));
	return true;
}

bool EventManager::unregister_handler(const EventHandler& handler,
	const EventType& type)
{
	bool success = false;
	auto handler_search = handlers.find(type);
	if (handler_search == handlers.end())
	{
		LOG_WARNING("Unregistering handler that is not registered.");
	}
	else
	{
		HandlerList& listeners = handler_search->second;
		for (auto i = listeners.begin(); i < listeners.end(); ++i)
		{
			if (handler == (*i))
			{
				listeners.erase(i);
				success = true;
				break;
			}
		}
	}
	LOG_TAGGED("Event", "Unregistered event handler " + std::to_string(type));
	return success;
}

bool EventManager::queue(EventPointer event)
{
	if (!event)
	{
		LOG_ERROR("Firing an invalid event");
		return false;
	}

	auto result = handlers.find(event->get_event_type());
	if (result == handlers.end())
	{
		LOG_TAGGED("Event", "Firing an event with no delegates, ignoring the " 
			+ std::string(event->get_name())
		);
		return false;
	}
	main_queue.push_back(event);
	
	LOG_TAGGED("Event", "Queued a(n) "
		+ std::string(event->get_name())
		+ " event with type " + std::to_string(event->get_event_type())
	);
	return true;
}

bool EventManager::queue_threadsafe(EventPointer event)
{
	if (!event)
	{
		LOG_ERROR("Firing an invalid event");
		return false;
	}

	threadsafe_queue.push(event);

	LOG_TAGGED("Event", "Queued a real time event of type "
		+ std::string(event->get_name())
	);
	return true;
}

void EventManager::fire_immediately(EventPointer event)
{
	auto list_result = handlers.find(event->get_event_type());

	LOG_ASSERT(list_result != handlers.end()
		&& "Firing an event we don't have handlers for");

	LOG_TAGGED("Event", "Firing an immediate event of type "
		+ std::string(event->get_name())
	);

	HandlerList& listeners = list_result->second;
	for (auto& listener : listeners)
	{
		listener(event);
	}
}

void EventManager::update(unsigned long max_milliseconds)
{
	Timestamp current = std::chrono::steady_clock::now();
	Timestamp end_time = current + std::chrono::milliseconds(max_milliseconds);

	EventPointer realtime_event;
	while (threadsafe_queue.try_pop(realtime_event))
	{
		queue(realtime_event);
		current = std::chrono::steady_clock::now();

		if (max_milliseconds != -1 && current > end_time)
		{
			LOG_ERROR("We have too many real time events to process");
		}
	}

	LOG_TAGGED("Event Loop", "Processing event queue containing "
		+ std::to_string(main_queue.size()) + " events");

	while (!main_queue.empty())
	{
		EventPointer event = main_queue.front();
		main_queue.pop_front();

		LOG_TAGGED("Event Loop", "Processing event "
			+ std::string(event->get_name()));

		const EventType& type = event->get_event_type();

		auto result = handlers.find(type);
		if (result == handlers.end())
		{
			LOG_TAGGED("Event Loop", "No handlers found, skipping");
		}
		else 
		{
			const HandlerList& listeners = result->second;
			LOG_TAGGED("Event Loop", "Found "
				+ std::to_string(listeners.size())
			+ " handlers");

			for (auto& listener : listeners)
			{
				listener(event);
			}
		}

		current = std::chrono::steady_clock::now();

		if (max_milliseconds != -1 && current > end_time)
		{
			LOG_TAGGED("Event Loop", "Ran out of time for event processing, aborting.");
			break;
		}
	}
}
