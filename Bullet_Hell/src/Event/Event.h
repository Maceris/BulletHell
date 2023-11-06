#pragma once

#include "Globals.h"

#include <memory>
#include <string>

class EventInformation
{};

class Event
{
public:
	virtual std::string event_type() = 0;
	virtual std::shared_ptr<EventInformation> get_information() = 0;
};

template<typename T>
concept IsEvent = std::is_base_of_v<Event, T>;
