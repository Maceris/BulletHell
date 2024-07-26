#pragma once

#include "map/tile.h"

struct Chunk;

namespace MapGenerator
{
	/// <summary>
	/// Calculate what the tile should be at the given coordinates.
	/// </summary>
	/// <param name="x">The x coordinate of the tile (global).</param>
	/// <param name="z">The z coordinate of the tile (global).</param>
	/// <returns>The type of tile that should be at the specified location.
	/// </returns>
	TileID get_tile(int x, int z);
	/// <summary>
	/// Populate a chunk with tiles. We expect the chunk's coordinates to
	/// be set, so that we can determine the tile coordinates. The 
	/// tile array will be overwritten by this.
	/// </summary>
	/// <param name="chunk">The chunk to fill with tiles, given its 
	/// coordinates.</param>
	void populate_chunk(Chunk& chunk);
}