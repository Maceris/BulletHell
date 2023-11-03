#include "Map.h"

Map::Map()
	: center{ 0 }
{
	for (int x = 0; x < LOADED_CHUNK_WIDTH; ++x)
	{
		for (int z = 0; z < LOADED_CHUNK_WIDTH; ++z)
		{
			loaded_chunks[x][z]->location.x = x - LOADED_CHUNKS_RADIUS;
			loaded_chunks[x][z]->location.z = z - LOADED_CHUNKS_RADIUS;

			MapGenerator::populate_chunk(*loaded_chunks[x][z]);
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
	//TODO(ches) Notify other systems the chunk was unloaded
	const ChunkCoordinates old_cooordinates = chunk->location;
	chunk->location = coordinates;
	MapGenerator::populate_chunk(*chunk);
	chunk_cache.push_front(chunk);
	cache_lookup.erase(old_cooordinates.combined);
	cache_lookup.emplace(std::make_pair(coordinates.combined, chunk));
	return chunk;
}

void Map::move_S()
{
	ScopedCriticalSection lock(chunk_critical_section);

	Chunk* temp;

	const uint16_t start_x = center.x - LOADED_CHUNKS_RADIUS;
	const uint16_t fresh_z = center.z + LOADED_CHUNKS_RADIUS + 1;

	for (int x = 0; x < LOADED_CHUNK_WIDTH; ++x)
	{
		temp = loaded_chunks[x][0];
		for (int z = 1; z < LOADED_CHUNK_WIDTH; ++z)
		{
			loaded_chunks[x][z - 1] = loaded_chunks[x][z];
		}

		ChunkCoordinates edge_position{start_x + x, fresh_z};
		Chunk* new_edge = get_cached(edge_position);
		loaded_chunks[x][LOADED_CHUNK_WIDTH - 1] = new_edge;
		/*
		 * NOTE(ches) since we just pulled from the cache, it's most recently
		 * used. We need to replace the entry in the cache (now the front)
		 * with the chunk that we just moved out of the main loaded chunks.
		 */
		chunk_cache.pop_front();
		chunk_cache.push_front(temp);
	}
	center.z++;
}