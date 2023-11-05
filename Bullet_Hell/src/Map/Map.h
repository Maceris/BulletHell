#pragma once

#include "Globals.h"

#include <list>
#include <memory>
#include <unordered_map>

#include "Chunk.h"
#include "CriticalSection.h"
#include "MapGenerator.h"

/// <summary>
/// Represents the radius (in chunks) of the fully loaded area, around the
/// player.
/// The loaded region is always square.
/// For example, a value of 1 means a 3x3 chunk area is fully loaded.
/// </summary>
constexpr auto HOT_CACHE_RADIUS = 1;

/// <summary>
/// Represents the radius (in chunks) of the partially loaded area, around the
/// player.
/// The loaded region is always square.
/// For example, a value of 1 means a 3x3 chunk area is at least partially
/// loaded.
/// </summary>
constexpr auto COLD_CACHE_RADIUS = HOT_CACHE_RADIUS + 1;

/// <summary>
/// The width, in chunks, of the fully loaded area around the player.
/// Calculated based on the radius.
/// </summary>
constexpr auto HOT_CACHE_CHUNK_WIDTH = 2 * HOT_CACHE_RADIUS + 1;

/// <summary>
/// The width, in chunks, of the partially loaded area around the player.
/// Calculated based on the radius.
/// </summary>
constexpr auto COLD_CACHE_CHUNK_WIDTH = 2 * COLD_CACHE_RADIUS + 1;

struct Map
{
	/// <summary>
	/// The coordinates of the central chunk, updated as we move around.
	/// </summary>
	ChunkCoordinates center;

	Map();
	Map(const Map&) = default;
	Map& operator=(const Map&) = default;
	~Map();

	/// <summary>
	/// Fetch the chunk for the given coordinates, generating it if required.
	/// </summary>
	/// <param name="coordinates">The coordinates to look for.</param>
	/// <returns>The chunk in that location.</returns>
	Chunk* get_cached(const ChunkCoordinates& coordinates);

	/// <summary>
	/// Ensure chunks are properly loaded after a player has moved to a new 
	/// chunk.
	/// </summary>
	/// <param name="old_center">The old location of the chunk the player
	/// was located in.</param>
	/// <param name="new_center">The location of the new chunk the player
	/// is now located in.</param>
	void constexpr recenter(const ChunkCoordinates& old_center,
		const ChunkCoordinates& new_center);

private:

	/// <summary>
	/// Ensure thread safety when modifying the chunks.
	/// </summary>
	CriticalSection chunk_critical_section;

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
	/// Calculate the coordinates that make up the fully-loaded region
	/// surrounding a specified chunk, and store them in the supplied vector.
	/// </summary>
	/// <param name="region_center">The coordinates of the chunk at the
	/// center of the region.</param>
	/// <param name="destination">Where to store the coordiantes of chunks
	/// that are in the fully loaded region.</param>
	void constexpr hot_region(const ChunkCoordinates& region_center,
		std::vector<ChunkCoordinates>& destination);

	/// <summary>
	/// Calculate the coordinates that make up the partially-loaded region
	/// surrounding a specified chunk, and store them in the supplied vector.
	/// </summary>
	/// <param name="region_center">The coordinates of the chunk at the
	/// center of the region.</param>
	/// <param name="destination">Where to store the coordiantes of chunks
	/// that are in the partially loaded region.</param>
	void constexpr cold_region(const ChunkCoordinates& region_center,
		std::vector<ChunkCoordinates>& destination);

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
	void cold_load(const ChunkCoordinates& coordinates);

	/// <summary>
	/// Load a chunk into the hot cache. If, for some reason, it's not in
	/// the cold cache yet, it will pass through the cold cache first.
	/// </summary>
	/// <param name="coordinates">The coordiante of the chunk.</param>
	void hot_load(const ChunkCoordinates& coordinates);

	/// <summary>
	/// Unload a chunk from the hot cache into the cold cache, in case we may
	/// need it in the hot cache again soon.
	/// </summary>
	/// <param name="coordinates">The coordiante of the chunk.</param>
	void cold_unload(const ChunkCoordinates& coordinates);

	/// <summary>
	/// Completely unload a chunk from either hot or cold caches.
	/// </summary>
	/// <param name="coordinates">The coordiante of the chunk.</param>
	void full_unload(const ChunkCoordinates& coordinates);
};