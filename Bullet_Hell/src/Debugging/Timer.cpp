#include "Debugging/Timer.h"

#include <map>

#include "boost/circular_buffer.hpp"

#include "Debugging/Logger.h"

/// <summary>
/// How many samples to keep in history for each timer, for calculating
/// averages.
/// </summary>
constexpr size_t HISTORY_LENGTH = 100;

/// <summary>
/// Used by the time manager to track times.
/// </summary>
struct Stopwatch
{
	Instant start;
	Instant end;
	long long elapsed;
	boost::circular_buffer<long long> history{ HISTORY_LENGTH };
};

class TimerManager
{
public:
	static std::map<std::string, Stopwatch> times;
	static std::vector<std::string> stages;
};

std::map<std::string, Stopwatch> TimerManager::times;
std::vector<std::string> TimerManager::stages;

void Timer::time_start(const std::string& stage_name, const Instant& instant)
{
	auto result = TimerManager::times.find(stage_name);
	if (result == TimerManager::times.end())
	{
		Stopwatch new_time{ instant, instant, 0 };
		TimerManager::times.emplace(stage_name, new_time);
		TimerManager::stages.push_back(stage_name);
	}
	else
	{
		result->second.start = instant;
	}
}

void Timer::time_end(const std::string& stage_name, const Instant& instant)
{
	auto result = TimerManager::times.find(stage_name);
	LOG_ASSERT(result != TimerManager::times.end()
		&& "Ending a stage that was not started");
	Stopwatch& stopwatch = result->second;
	stopwatch.end = instant;
	stopwatch.elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(
			stopwatch.end - stopwatch.start).count();
	stopwatch.history.push_back(stopwatch.elapsed);
}

long long Timer::last_time(const std::string& stage_name)
{
	auto result = TimerManager::times.find(stage_name);
	LOG_ASSERT(result != TimerManager::times.end()
		&& "Timing a stage that was not started");

	return result->second.elapsed;
}

long long Timer::average_time(const std::string& stage_name)
{
	auto result = TimerManager::times.find(stage_name);
	LOG_ASSERT(result != TimerManager::times.end()
		&& "Timing a stage that was not started");
	
	Stopwatch& stopwatch = result->second;

	//NOTE(ches) these could get big, so we want to avoid overflow
	long long nearly_average = 0;
	long long remainders = 0;
	const size_t length = std::min(HISTORY_LENGTH, stopwatch.history.size());
	
	if (length == 1)
	{
		return stopwatch.elapsed;
	}

	for (size_t i = 0; i < length; ++i)
	{
		nearly_average += stopwatch.history[i] / length;
		remainders += stopwatch.history[i] % length;
	}
	return nearly_average + (remainders / length);
}

const std::vector<std::string>& Timer::time_stages_list()
{
	return TimerManager::stages;
}