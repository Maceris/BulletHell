#include "EventManager.h"

#include "Logger.h"

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
	LOG_TAGGED("Event", "Registered event handler " + std::to_string(type));
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
	LOG_ASSERT(active_queue > 0);
	LOG_ASSERT(active_queue < EVENT_QUEUE_COUNT);

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
	queues[active_queue].push(event);
	
	LOG_TAGGED("Event", "Queued an event of type "
		+ std::string(event->get_name())
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

void update(unsigned long max_milliseconds)
{
	Timestamp current = std::chrono::steady_clock::now();
	Timestamp end_time = current + std::chrono::milliseconds(max_milliseconds);

}
