#pragma once

#include "Globals.h"

#include "Chunk.h"

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

class Map
{
	/// <summary>
	/// The x coordinate of the central chunk, updated as we move around.
	/// </summary>
	ChunkCoordinate center_x;

	/// <summary>
	/// The z coordinate of the central chunk, updated as we move around.
	/// </summary>
	ChunkCoordinate center_z;

	/// <summary>
	/// The currently loaded chunks. They are stored in column-major
	/// order, with the first coordinate being the x axis, and the second
	/// being the z axis.
	/// </summary>
	Chunk loaded_chunks[LOADED_CHUNK_WIDTH][LOADED_CHUNK_WIDTH];
};