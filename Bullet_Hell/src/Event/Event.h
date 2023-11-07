#pragma once

#include "Globals.h"

#include <chrono>
#include <memory>

/// <summary>
/// A unique identifier for a specific type of event.
/// </summary>
using EventType = unsigned long;

/// <summary>
/// An event timestamp, when the event was fired.
/// </summary>
using Timestamp = std::chrono::steady_clock::time_point;

/// <summary>
/// Event data representing something that happened in the game.
/// </summary>
class Event
{
public:
	/// <summary>
	/// Fetch the unique identifier for the type of event this is.
	/// </summary>
	/// <returns>The type of event.</returns>
	virtual const EventType& get_event_type() const = 0;
	/// <summary>
	/// Returns a timestamp representing the current system time in 
	/// milliseconds when the event was fired.
	/// </summary>
	/// <returns>The timestamp from when the event was fired.</returns>
	virtual Timestamp get_timestamp() const = 0;
	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	virtual const char* get_name() const = 0;
};

/// <summary>
/// A base implementation of an event that includes a timestamp.
/// </summary>
class BaseEvent : public Event
{
private:
	const Timestamp timestamp;

public:
	explicit BaseEvent()
		: timestamp { std::chrono::steady_clock::now() }
	{}
	explicit BaseEvent(const Timestamp timestamp)
		: timestamp{ timestamp }
	{}
	virtual ~BaseEvent() {}

	virtual const EventType& get_event_type() const = 0;
	Timestamp get_timestamp() const
	{
		return timestamp;
	}
};

/// <summary>
/// Whether a type is a subtype of event.
/// </summary>
template<typename T>
concept IsEvent = std::is_base_of_v<Event, T>;

/// <summary>
/// A pointer to an event.
/// </summary>
using EventPointer = std::shared_ptr<Event>;