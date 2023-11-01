#pragma once

#include "Globals.h"

#include "Tile.h"

/// <summary>
/// The number of tiles per side of a chunk.
/// </summary>
constexpr int CHUNK_WIDTH = 16;

/// <summary>
/// Coordinate of a chunk within the world.
/// </summary>
using ChunkCoordinate = int16_t;

/// <summary>
/// A cluster of tiles.
/// </summary>
struct Chunk
{
	/// <summary>
	/// The x coordinate of the chunk, measured in chunks.
	/// </summary>
	ChunkCoordinate x;

	/// <summary>
	/// The z coordinate of the chunk, measured in chunks.
	/// </summary>
	ChunkCoordinate z;

	/// <summary>
	/// The tiles that make up the chunk. They are stored in column-major
	/// order, with the first coordinate being the x axis, and the second
	/// being the z axis.
	/// 
	/// These tiles start at the lowest x and y coordinates, and go up towards
	/// the +x and +z axis.
	/// </summary>
	Tile tiles[CHUNK_WIDTH][CHUNK_WIDTH];
};