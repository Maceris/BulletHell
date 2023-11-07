#pragma once

#include "Event.h"

#include <map>
#include <vector>

#include "ConcurrentQueue.h"
#include "Delegate.h"

/// <summary>
/// A delegate that will process an event when passed one.
/// </summary>
using EventHandler = SA::delegate<void(EventPointer)>;

/// <summary>
/// A list of delegates to call when processing an event.
/// </summary>
using HandlerList = std::vector<EventHandler>;

/// <summary>
/// A queue of events that need to be fired.
/// </summary>
using EventQueue = std::queue<EventPointer>;

/// <summary>
/// A threadsafe queue used to register events from another thread.
/// </summary>
using ThreadSafeEventQueue = ConcurrentQueue<EventPointer>;

/// <summary>
/// How many event queues we have.
/// </summary>
constexpr unsigned int EVENT_QUEUE_COUNT = 2;

class EventManager
{
public:
	/// <summary>
	/// Register an event handler.
	/// </summary>
	/// <param name="handler">The event delegate.</param>
	/// <param name="type">The type of event this delegate is for.</param>
	bool register_handler(const EventHandler& handler, const EventType& type);
	
	/// <summary>
	/// Unregister an event handler.
	/// </summary>
	/// <param name="handler">The event delegate.</param>
	/// <param name="type">The type of event this delegate is for.</param>
	/// <returns>Whether we successfully unregistered the handler.</returns>
	bool unregister_handler(const EventHandler& handler, 
		const EventType& type);
	
	/// <summary>
	/// Add an event to the queue.
	/// </summary>
	/// <param name="event">The event to fire.</param>
	/// <returns>Whether we successfully enqueued the event.</returns>
	bool queue(EventPointer event);

	// <summary>
	/// Add an event to the queue from another thread. These will be enqueued
	/// to the active queue when processing.
	/// </summary>
	/// <param name="event">The event to fire.</param>
	/// <returns>Whether we successfully enqueued the event.</returns>
	bool queue_threadsafe(EventPointer event);

	/// <summary>
	/// Send an event to delegates immediately, bypassing the queues.
	/// </summary>
	/// <param name="event">The event to fire.</param>
	void fire_immediately(EventPointer event);

	/// <summary>
	/// Go through and fire events in the active queue.
	/// </summary>
	void update(unsigned long max_milliseconds);
private:

	/// <summary>
	/// The list of delegates currently registered for each type of event.
	/// </summary>
	std::map<EventType, HandlerList> handlers;

	/// <summary>
	/// The event queues.
	/// </summary>
	EventQueue queues[EVENT_QUEUE_COUNT];

	/// <summary>
	/// The queue for events from another thread.
	/// </summary>
	ThreadSafeEventQueue threadsafe_queue;

	/// <summary>
	/// Which queue is currently active. Events are always queued on an
	/// inactive queue.
	/// </summary>
	unsigned int active_queue;
};