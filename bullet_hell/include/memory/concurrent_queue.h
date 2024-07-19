#pragma once

#include <queue>

#include "memory/critical_section.h"

/// <summary>
/// A concurrent queue. Written by Anthony Williams, found at
/// http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
/// </summary>
/// <typeparam name="T">The type of elements in the queue.</typeparam>
template<typename T>
class ConcurrentQueue
{
private:
	std::queue<T> queue;
	CriticalSection critical_section;
	HANDLE data_pushed;
public:
	ConcurrentQueue()
	{
		const BOOL manual_reset = TRUE;
		const BOOL initial_state = FALSE;
		data_pushed = CreateEvent(nullptr, manual_reset, initial_state, nullptr);
	}

	/// <summary>
	/// Push to the queue.
	/// </summary>
	/// <param name="data">The data to add to the queue.</param>
	void push(T const& data)
	{
		{
			ScopedCriticalSection lock(critical_section);
			queue.push(data);
		}
		PulseEvent(data_pushed);
	}

	/// <summary>
	/// Checks if the queue is empty.
	/// </summary>
	/// <returns>Whether the queue is empty.</returns>
	bool empty()
	{
		ScopedCriticalSection lock(critical_section);
		return queue.empty();
	}

	/// <summary>
	/// Attempt to pop from the queue, and store the result in the parameter.
	/// </summary>
	/// <param name="result">Where to store the result.</param>
	/// <returns>Whether we were able to pop from the queue.</returns>
	bool try_pop(T& result)
	{
		ScopedCriticalSection lock(critical_section);
		if (queue.empty())
		{
			return false;
		}

		result = queue.front();
		queue.pop();
		return true;
	}

	/// <summary>
	/// Wait until the queue has at least one element in it, and then finally 
	/// pop it and return it in the parameter.
	/// </summary>
	/// <param name="result">Where to store the result.</param>
	void wait_and_pop(T& result)
	{
		ScopedCriticalSection lock(critical_section);
		while (queue.empty())
		{
			WaitForSingleObject(data_pushed, 20);
		}
		result = queue.front();
		queue.pop();
	}
};