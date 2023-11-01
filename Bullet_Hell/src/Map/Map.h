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
	/// The currently loaded chunks. They are stored in column-major
	/// order, with the first coordinate being the x axis, and the second
	/// being the z axis.
	/// </summary>
	Chunk loaded_chunks[LOADED_CHUNK_WIDTH][LOADED_CHUNK_WIDTH];

	Map();
	Map(const Map&) = default;
	Map& operator=(const Map&) = default;
	~Map();

private:

	/// <summary>
	/// Ensure thread safety when modifying the chunks.
	/// </summary>
	CriticalSection chunk_critical_section;

	/// <summary>
	/// Where cached chunks are actually stored.
	/// </summary>
	std::list<Chunk*> chunk_cache;

	/// <summary>
	/// Used to look up
	/// </summary>
	std::unordered_map<uint32_t, Chunk*> cache_lookup;

	/// <summary>
	/// Fetch the chunk for the given coordinates, generating it if required.
	/// </summary>
	/// <param name="coordinates">The coordinates to look for.</param>
	/// <returns>The chunk in that location.</returns>
	Chunk* get_cached(const ChunkCoordinates& coordinates);
};