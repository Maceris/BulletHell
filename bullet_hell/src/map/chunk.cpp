#include "Map/Chunk.h"

Chunk::Chunk()
	: location{ 0 }
	, tiles { TILE_VOID }
{}

Chunk::Chunk(const ChunkCoordinates& coordinates)
	: location{ coordinates }
	, tiles{ TILE_VOID }
{}
