#pragma once

#include "globals.h"

#include <chrono>
#include <string>
#include <vector>

using Instant = std::chrono::steady_clock::time_point;

namespace Timer
{
	/// <summary>
	/// Clear out all of the timers history except the last recorded time.
	/// </summary>
	void clear_timer_history();

	/// <summary>
	/// Marks down the instant we start timing a named section of code, like a 
	/// stopwatch.
	/// </summary>
	/// <param name="stage_name">The name of what we are timing.</param>
	/// <param name="instant">When we started timing.</param>
	void time_start(const std::string& stage_name, const Instant& instant);

	/// <summary>
	/// Marks down the instant we start timing a named section of code, like a 
	/// stopwatch.
	/// </summary>
	/// <param name="stage_name">The name of what we are timing.</param>
	/// <param name="instant">When we stopped timing.</param>
	void time_end(const std::string& stage_name, const Instant& instant);

	/// <summary>
	/// Calculates the difference in time, in microseconds, between the last
	/// time we started and ended a timer.
	/// </summary>
	/// <param name="stage_name">The name of what we were timing.</param>
	/// <returns>The number of microseconds between start and end.</returns>
	long long last_time(const std::string& stage_name);

	/// <summary>
	/// Calculates the average difference in time, in microseconds, between
	/// for a timer. This is a rolling average based on recent history.
	/// </summary>
	/// <param name="stage_name">The name of what we were timing.</param>
	/// <returns>The average time in microseconds between start and end.
	/// </returns>
	long long average_time(const std::string& stage_name);

	/// <summary>
	/// Fetch a list of stage names.
	/// </summary>
	/// <returns>The names of all currently used stages.</returns>
	const std::vector<std::string>& time_stages_list();
}

#if _DEBUG

/// <summary>
/// Marks down the instant we start timing a named section of code, like a
/// stopwatch.
/// </summary>
#define TIME_START(str) \
	do \
	{ \
		std::string s((str)); \
		Instant now = std::chrono::steady_clock::now(); \
		Timer::time_start(s, now); \
	} \
	while (0) \

/// <summary>
/// Stop timing a named section of code.
/// </summary>
#define TIME_END(str) \
	do \
	{ \
		std::string s((str)); \
		Instant now = std::chrono::steady_clock::now(); \
		Timer::time_end(s, now); \
	} \
	while (0) \

/// <summary>
/// Fetch the elapsed time for a named section of code, in microseconds.
/// </summary>
#define LAST_TIME(str) Timer::last_time(std::string((str)))

/// <summary>
/// Fetch the average time for a named section of code, in microseconds.
/// This is a rolling average based on recent history.
/// </summary>
#define AVERAGE_TIME(str) Timer::average_time(std::string((str)))

/// <summary>
/// Fetch a list of stage names.
/// </summary>
#define TIME_STAGES_LIST Timer::time_stages_list()

/// <summary>
/// Clear out all of the timers history except the last recorded time.
/// </summary>
#define CLEAR_TIMER_HISTORY() Timer::clear_timer_history()

#else // _DEBUG

#define TIME_START(str) do { (void)sizeof(str); } while (0)
#define TIME_END(str) do { (void)sizeof(str); } while (0)
#define LAST_TIME(str) 0
#define AVERAGE_TIME(str) 0
#define TIME_STAGES_LIST Timer::time_stages_list()
#define CLEAR_TIMER_HISTORY() do { (void)sizeof(str); } while (0)

#endif // _DEBUG else