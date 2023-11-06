#pragma once

#include "Event.h"

#include <unordered_map>
#include <vector>

#include "MultiCastDelegate.h"

using EventHandler = SA::delegate<void(Event& event)>;

using HandlerList = std::vector<EventHandler>;

class EventManager
{
public:
	void register_handler(const std::string& event_name,
		EventHandler handler);

	void unregister_handlers(const std::string& event_name);

	void fire(Event& event);
private:
	std::unordered_map<std::string, HandlerList> handlers;
};