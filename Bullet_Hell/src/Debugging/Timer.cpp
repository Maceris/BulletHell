#include "Timer.h"

#include <map>

#include "Logger.h"

/// <summary>
/// Used by the time manager to track times.
/// </summary>
struct Stopwatch
{
	Instant start;
	Instant end;
	long long elapsed;
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
	result->second.end = instant;
	result->second.elapsed = 
		std::chrono::duration_cast<std::chrono::microseconds>(
			result->second.end - result->second.start).count();
}

long long Timer::last_time(const std::string& stage_name)
{
	auto result = TimerManager::times.find(stage_name);
	LOG_ASSERT(result != TimerManager::times.end()
		&& "Timing a stage that was not started");

	return result->second.elapsed;
}

const std::vector<std::string>& Timer::time_stages_list()
{
	return TimerManager::stages;
}