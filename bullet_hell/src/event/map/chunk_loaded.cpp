#include "Event/Map/ChunkLoaded.h"

#include "Map/Chunk.h"

const EventType ChunkLoaded::event_type = 0xf8b52643;

ChunkLoaded::ChunkLoaded(const Chunk* chunk)
	: chunk{ chunk }
{}
