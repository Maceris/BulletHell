#pragma once

#include "Globals.h"

#include <list>
#include <memory>
#include <unordered_map>

#include "Chunk.h"
#include "CriticalSection.h"
#include "MapGenerator.h"

/// <summary>
/// Represents the radius (in chunks) of the loaded area, around the player.
/// The loaded region is always square.
/// For example, a value of 1 means a 3x3 chunk area is loaded, the chunk
/// that the player is in, and 1 more in each cardinal direction and diagonals.
/// </summary>
constexpr auto LOADED_CHUNKS_RADIUS = 1;

/// <summary>
/// The width, in chunks, of the loaded area around the player. Calculated
/// based on the radius.
/// </summary>
constexpr auto LOADED_CHUNK_WIDTH = 2 * LOADED_CHUNKS_RADIUS + 1;

/// <summary>
/// How many cached chunks we keep around.
/// </summary>
constexpr auto CACHE_SIZE = (LOADED_CHUNK_WIDTH + 2) * 4;

struct Map
{
	/// <summary>
	/// The coordinates of the central chunk, updated as we move around.
	/// </summary>
	ChunkCoordinates center;

	/// <summary>
	/// The currently loaded chunks. They are stored in with the first
	/// coordinate being the x axis, and the second being the z axis.
	/// </summary>
	Chunk* loaded_chunks[LOADED_CHUNK_WIDTH][LOADED_CHUNK_WIDTH];

	Map();
	Map(const Map&) = default;
	Map& operator=(const Map&) = default;
	~Map();

private:

	/// <summary>
	/// Ensure thread safety when modifying the chunks.
	/// </summary>
	CriticalSection chunk_critical_section;

	//TODO(ches) remove this
	std::list<Chunk*> chunk_cache;

	//TODO(ches) remove this
	std::unordered_map<uint32_t, Chunk*> cache_lookup;

	/// <summary>
	/// Chunks that are loaded, but not completely. We know about the tiles
	/// but there are no meshes or lights loaded for the chunks.
	/// </summary>
	std::unordered_map<uint32_t, Chunk*> cold_cache;

	/// <summary>
	/// Chunks that are fully loaded, and able to be rendered.
	/// </summary>
	std::unordered_map<uint32_t, Chunk*> hot_cache;

	/// <summary>
	/// Fetch the chunk for the given coordinates, generating it if required.
	/// </summary>
	/// <param name="coordinates">The coordinates to look for.</param>
	/// <returns>The chunk in that location.</returns>
	Chunk* get_cached(const ChunkCoordinates& coordinates);

	void constexpr recenter(const ChunkCoordinates& old_center,
		const ChunkCoordinates& new_center);

	/// <summary>
	/// Check if the supplied coordinates are in the cold cache.
	/// </summary>
	/// <param name="coordinates">The coordinates to look for.</param>
	/// <returns>Whether the given coordinates are currently in the cold cache.
	/// </returns>
	bool is_cold(const ChunkCoordinates& coordinates);

	/// <summary>
	/// Check if the supplied coordinates are in the hot cache.
	/// </summary>
	/// <param name="coordinates">The coordinates to look for.</param>
	/// <returns>Whether the given coordinates are currently in the hot cache.
	/// </returns>
	bool is_hot(const ChunkCoordinates& coordinates);

	/// <summary>
	/// Load a chunk into the cold cache.
	/// </summary>
	/// <param name="coordinates">The coordiante of the chunk.</param>
	void constexpr cold_load(const ChunkCoordinates& coordinates);

	/// <summary>
	/// Load a chunk into the hot cache. If, for some reason, it's not in
	/// the cold cache yet, it will pass through the cold cache first.
	/// </summary>
	/// <param name="coordinates">The coordiante of the chunk.</param>
	void constexpr hot_load(const ChunkCoordinates& coordinates);

	/// <summary>
	/// Unload a chunk from the hot cache into the cold cache, in case we may
	/// need it in the hot cache again soon.
	/// </summary>
	/// <param name="coordinates">The coordiante of the chunk.</param>
	void constexpr cold_unload(const ChunkCoordinates& coordinates);

	/// <summary>
	/// Completely unload a chunk from either hot or cold caches.
	/// </summary>
	/// <param name="coordinates">The coordiante of the chunk.</param>
	void constexpr full_unload(const ChunkCoordinates& coordinates);
	
	//TODO(ches) remove this
	void move_S();

};