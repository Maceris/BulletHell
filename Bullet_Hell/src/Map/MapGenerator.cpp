#include "MapGenerator.h"

/// <summary>
/// Calculate the global coordinate of a tile, based on the chunk coordinate
/// that it is in, and it's local cooridinates within the chunk.
/// </summary>
/// <param name="chunk_coordinate">The coordinate of the chunk the tile is in.
/// </param>
/// <param name="local_coordinate">The local coordinate of the tile within
/// the chunk.</param>
/// <returns>The global coordinates of the tile.</returns>
int constexpr global_coordinate(ChunkCoordinate chunk_coordinate,
	unsigned int local_coordinate)
{
	return chunk_coordinate * CHUNK_WIDTH + local_coordinate;
}

TileID MapGenerator::get_tile(int x, int y)
{
	//TODO(ches) actually generate tiles
	return TILE_GROUND;
}

void MapGenerator::populate_chunk(Chunk& chunk)
{
	const int start_x = chunk.x * CHUNK_WIDTH;
	const int end_x = (chunk.x + 1) * CHUNK_WIDTH - 1;
	const int start_z = chunk.z * CHUNK_WIDTH;
	const int end_z = (chunk.z + 1) * CHUNK_WIDTH - 1;

	for (int x = start_x; x <= end_x; ++x)
	{
		for (int z = start_z; z <= end_z; ++z)
		{
			chunk.tiles[x][z].id = get_tile(start_x + x, start_z + z);
		}
	}
}
