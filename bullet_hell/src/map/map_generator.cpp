#include "Map/MapGenerator.h"

#include "PerlinNoise.hpp"

#include "Chunk.h"

const siv::PerlinNoise::seed_type seed = 1;

const siv::BasicPerlinNoise<float> perlin{ seed };

/// <summary>
/// Calculate the global coordinate of a tile, based on the chunk coordinate
/// that it is in, and it's local cooridinates within the chunk.
/// </summary>
/// <param name="chunk_coordinate">The coordinate of the chunk the tile is in.
/// </param>
/// <param name="local_coordinate">The local coordinate of the tile within
/// the chunk.</param>
/// <returns>The global coordinates of the tile.</returns>
int constexpr global_coordinate(uint16_t chunk_coordinate,
	unsigned int local_coordinate)
{
	return chunk_coordinate * CHUNK_WIDTH + local_coordinate;
}

TileID MapGenerator::get_tile(int x, int y)
{
	const float noise = perlin.octave2D(x, y, 4);

	if (noise <= 0)
	{
		return TILE_GROUND;
	}

	return TILE_GROUND_DIRTY;
}

void MapGenerator::populate_chunk(Chunk& chunk)
{
	const int start_x = chunk.location.x * CHUNK_WIDTH;
	const int end_x = (chunk.location.x + 1) * CHUNK_WIDTH - 1;
	const int start_z = chunk.location.z * CHUNK_WIDTH;
	const int end_z = (chunk.location.z + 1) * CHUNK_WIDTH - 1;

	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int z = 0; z < CHUNK_WIDTH; ++z)
		{
			chunk.tiles[x][z].id = get_tile(start_x + x, start_z + z);
		}
	}
}
