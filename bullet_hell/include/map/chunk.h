#pragma once

#include "map/chunk_coordinates.h"
#include "map/tile.h"

/// <summary>
/// The number of tiles per side of a chunk.
/// </summary>
constexpr int CHUNK_WIDTH = 16;

/// <summary>
/// A cluster of tiles.
/// </summary>
struct Chunk
{
	/// <summary>
	/// The x and z coordinates of the chunk, measured in chunks.
	/// </summary>
	ChunkCoordinates location;

	/// <summary>
	/// The tiles that make up the chunk. They are stored with the first
	/// coordinate being the x axis, and the second being the z axis.
	/// 
	/// These tiles start at the lowest x and y coordinates, and go up towards
	/// the +x and +z axis.
	/// </summary>
	Tile tiles[CHUNK_WIDTH][CHUNK_WIDTH];

	Chunk();
	Chunk(const ChunkCoordinates& coordinates);
	Chunk(const Chunk&) = default;
	Chunk& operator=(const Chunk&) = default;
	~Chunk() = default;
};