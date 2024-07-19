#include "event/map/chunk_unloaded.h"

#include "map/chunk.h"

const EventType ChunkUnloaded::event_type = 0x16ae5d01;

ChunkUnloaded::ChunkUnloaded(const ChunkCoordinates coordinates, 
	const Chunk* chunk)
	: chunk{ chunk }
	, coordinates{ coordinates }
{}
