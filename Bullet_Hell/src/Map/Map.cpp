#include "Map.h"

Map::Map()
	: center{ 0 }
{

	for (int x = 0; x < LOADED_CHUNK_WIDTH; ++x)
	{
		for (int z = 0; z < LOADED_CHUNK_WIDTH; ++z)
		{
			loaded_chunks[x][z].location.x = x - LOADED_CHUNKS_RADIUS;
			loaded_chunks[x][z].location.z = z - LOADED_CHUNKS_RADIUS;

			MapGenerator::populate_chunk(loaded_chunks[x][z]);
		}
	}
}

Map::~Map()
{
	while (!chunk_cache.empty())
	{
		Chunk* chunk = chunk_cache.front();
		chunk_cache.pop_front();
		SAFE_DELETE(chunk);
	}
	cache_lookup.clear();
}

Chunk* Map::get_cached(const ChunkCoordinates& coordinates)
{
	ScopedCriticalSection lock(chunk_critical_section);

	const auto result = cache_lookup.find(coordinates.combined);
	
	if (result != cache_lookup.end())
	{
		Chunk* chunk = result->second;
		//NOTE(ches) move to the front of the cache.
		chunk_cache.remove(chunk);
		chunk_cache.push_front(chunk);
		return chunk;
	}

	//NOTE(ches) Not in the cache, but we have room to add it
	if (chunk_cache.size() < CACHE_SIZE)
	{
		Chunk* chunk = ALLOC Chunk(coordinates);
		MapGenerator::populate_chunk(*chunk);
		chunk_cache.push_front(chunk);
		cache_lookup.emplace(std::make_pair(coordinates.combined, chunk));
		return chunk;
	}

	//NOTE(ches) We have to reuse a chunk.
	Chunk* chunk = chunk_cache.back();//NOTE(ches) least recently used
	chunk_cache.pop_back();
	const ChunkCoordinates old_cooordinates = chunk->location;
	chunk->location = coordinates;
	MapGenerator::populate_chunk(*chunk);
	chunk_cache.push_front(chunk);
	cache_lookup.erase(old_cooordinates.combined);
	cache_lookup.emplace(std::make_pair(coordinates.combined, chunk));
	return chunk;
}
