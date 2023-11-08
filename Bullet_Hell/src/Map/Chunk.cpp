#include "Map/Chunk.h"

Chunk::Chunk()
	: location{ 0 }
{}

Chunk::Chunk(const ChunkCoordinates& coordinates)
	: location{ coordinates }
{}
