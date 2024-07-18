#include "Event/Map/ChunkUnloaded.h"

#include "Map/Chunk.h"

const EventType ChunkUnloaded::event_type = 0x16ae5d01;

ChunkUnloaded::ChunkUnloaded(const ChunkCoordinates coordinates, 
	const Chunk* chunk)
	: chunk{ chunk }
	, coordinates{ coordinates }
{}
