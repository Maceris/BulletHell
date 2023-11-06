#include "EventManager.h"

#include "Logger.h"

void EventManager::register_handler(const std::string& event_name, 
	EventHandler handler)
{
	auto result = handlers.find(event_name);

	if (result == handlers.end())
	{
		handlers.insert(std::make_pair(event_name, HandlerList()));
		result = handlers.find(event_name);
	}

	HandlerList& handlers = result->second;
	handlers.push_back(handler);
}

void EventManager::unregister_handlers(const std::string& event_name)
{
	handlers.erase(event_name);
}

void EventManager::fire(Event& event)
{
	auto list_result = handlers.find(event.event_type());

	LOG_ASSERT(list_result != handlers.end()
		&& "Firing an event we don't have handlers for");

	for (auto& process : list_result->second)
	{
		process(event);
	}
}