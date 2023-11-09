#include "Map/GameMap.h"

#include "Debugging/Logger.h"
#include "Event/Map/ChunkLoaded.h"
#include "Event/Map/ChunkUnloaded.h"
#include "Main/GameLogic.h"
#include "Memory/CriticalSection.h"

/// <summary>
/// Ensure thread safety when modifying the chunks.
/// </summary>
CriticalSection chunk_critical_section;

GameMap::GameMap()
	: center{ 0 }
{
	ScopedCriticalSection lock(chunk_critical_section);
	std::vector<ChunkCoordinates> hot_list;
	hot_region(center, hot_list);

	for (ChunkCoordinates& coordinates : hot_list)
	{
		hot_load(coordinates);
	}

	std::vector<ChunkCoordinates> cold_list;
	cold_region(center, cold_list);

	for (ChunkCoordinates& coordinates : cold_list)
	{
		cold_load(coordinates);
	}
}

GameMap::~GameMap()
{
	ScopedCriticalSection lock(chunk_critical_section);
	for (auto it = hot_cache.begin(); it != hot_cache.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}
	hot_cache.clear();

	for (auto it = cold_cache.begin(); it != cold_cache.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}
	cold_cache.clear();
}

Chunk* GameMap::get_cached(const ChunkCoordinates& coordinates)
{
	ScopedCriticalSection lock(chunk_critical_section);

	const auto hot_result = hot_cache.find(coordinates.combined);
	
	if (hot_result != hot_cache.end())
	{
		return hot_result->second;
	}

	const auto cold_result = cold_cache.find(coordinates.combined);

	if (cold_result != cold_cache.end())
	{
		return cold_result->second;
	}

	hot_load(coordinates);

	const auto fresh_result = hot_cache.find(coordinates.combined);

	LOG_ASSERT(fresh_result != hot_cache.end()
		&& "We failed to load a chunk after finding it missing");

	return fresh_result->second;
}

bool GameMap::is_cold(const ChunkCoordinates& coordinates)
{
	return cold_cache.find(coordinates.combined) != cold_cache.end();
}

bool GameMap::is_hot(const ChunkCoordinates& coordinates)
{
	return hot_cache.find(coordinates.combined) != cold_cache.end();
}

void constexpr GameMap::hot_region(const ChunkCoordinates& region_center,
	std::vector<ChunkCoordinates>& destination)
{
	const int16_t start_x = region_center.x - HOT_CACHE_RADIUS;
	const int16_t end_x = region_center.x + HOT_CACHE_RADIUS;
	const int16_t start_z = region_center.z - HOT_CACHE_RADIUS;
	const int16_t end_z = region_center.z + HOT_CACHE_RADIUS;

	for (int16_t x = start_x; x <= end_x; ++x)
	{
		for (int16_t z = start_z; z <= end_z; ++z)
		{
			destination.emplace_back(x, z);
		}
	}
}

void constexpr GameMap::cold_region(const ChunkCoordinates& region_center,
	std::vector<ChunkCoordinates>& destination)
{
	const int16_t start_x = region_center.x - COLD_CACHE_RADIUS;
	const int16_t end_x = region_center.x + COLD_CACHE_RADIUS;
	const int16_t start_z = region_center.z - COLD_CACHE_RADIUS;
	const int16_t end_z = region_center.z + COLD_CACHE_RADIUS;

	const int16_t start_x_ignore = region_center.x - HOT_CACHE_RADIUS;
	const int16_t end_x_ignore = region_center.x + HOT_CACHE_RADIUS;
	const int16_t start_z_ignore = region_center.z - HOT_CACHE_RADIUS;
	const int16_t end_z_ignore = region_center.z + HOT_CACHE_RADIUS;

	for (int16_t x = start_x; x <= end_x; ++x)
	{
		for (int16_t z = start_z; z <= end_z; ++z)
		{
			destination.emplace_back(x, z);
		}
	}
}

void constexpr GameMap::recenter(const ChunkCoordinates& old_center,
	const ChunkCoordinates& new_center)
{
	ScopedCriticalSection lock(chunk_critical_section);
	std::vector<ChunkCoordinates> old_hot;
	std::vector<ChunkCoordinates> old_cold;
	std::vector<ChunkCoordinates> new_hot;
	std::vector<ChunkCoordinates> new_cold;

	hot_region(old_center, old_hot);
	hot_region(new_center, new_hot);
	cold_region(old_center, old_cold);
	cold_region(new_center, new_cold);

	std::vector<ChunkCoordinates> need_full_loading;
	std::vector<ChunkCoordinates> need_partial_loading;

	std::vector<ChunkCoordinates> need_partial_unloading;
	std::vector<ChunkCoordinates> need_full_unloading;
	
	for (const auto& desired : new_hot)
	{
		bool was_already_hot = false;
		for (const auto& old : old_hot)
		{
			if (old == desired)
			{
				was_already_hot = true;
				break;
			}
		}
		if (was_already_hot)
		{
			continue;
		}
		need_full_loading.push_back(desired);
	}

	for (const auto& desired : new_cold)
	{
		bool was_already_cold = false;
		for (const auto& old : old_cold)
		{
			if (old == desired)
			{
				was_already_cold = true;
				break;
			}
		}
		if (was_already_cold)
		{
			continue;
		}

		bool was_hot = false;
		for (const auto& old : old_hot)
		{
			if (old == desired)
			{
				was_hot = true;
				break;
			}
		}
		if (was_hot)
		{
			need_partial_unloading.push_back(desired);
		}
		else
		{
			need_partial_loading.push_back(desired);
		}
	}

	for (const auto& to_load : need_full_loading)
	{
		hot_load(to_load);
	}

	for (const auto& to_load : need_partial_loading)
	{
		cold_load(to_load);
	}

	for (const auto& to_load : need_partial_unloading)
	{
		cold_unload(to_load);
	}

	for (const auto& to_load : need_full_unloading)
	{
		full_unload(to_load);
	}
}

void GameMap::cold_load(const ChunkCoordinates& coordinates)
{
	Chunk* fresh = ALLOC Chunk(coordinates);
	MapGenerator::populate_chunk(*fresh);
	cold_cache.insert(std::make_pair(coordinates.combined, fresh));
}

void GameMap::hot_load(const ChunkCoordinates& coordinates)
{
	if (!is_cold(coordinates))
	{
		cold_load(coordinates);
	}

	auto cold_chunk = cold_cache.find(coordinates.combined);
	LOG_ASSERT(cold_chunk != cold_cache.end()
		&& "We failed to cold load a chunk");

	Chunk* loaded = cold_chunk->second;
	cold_cache.erase(coordinates.combined);
	hot_cache.insert(std::make_pair(coordinates.combined, loaded));

	g_event_manager->queue(std::make_shared<ChunkLoaded>(loaded));
}

void GameMap::cold_unload(const ChunkCoordinates& coordinates)
{
	if (!is_hot(coordinates))
	{
		return;
	}
	
	auto hot_chunk = hot_cache.find(coordinates.combined);
	LOG_ASSERT(hot_chunk != hot_cache.end()
		&& "Chunk not found despite reportedly being loaded");

	Chunk* loaded = hot_chunk->second;
	hot_cache.erase(coordinates.combined);
	cold_cache.insert(std::make_pair(coordinates.combined, loaded));

	g_event_manager->queue(std::make_shared<ChunkUnloaded>(coordinates, 
		loaded));
}

void GameMap::full_unload(const ChunkCoordinates& coordinates)
{
	if (is_hot(coordinates))
	{
		cold_unload(coordinates);
	}

	if (!is_cold(coordinates))
	{
		return;
	}

	auto cold_chunk = cold_cache.find(coordinates.combined);
	LOG_ASSERT(cold_chunk != cold_cache.end()
		&& "Chunk not found despite reportedly being loaded");

	Chunk* loaded = cold_chunk->second;
	cold_cache.erase(coordinates.combined);
	SAFE_DELETE(loaded);
}