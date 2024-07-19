#include "event/map/chunk_loaded.h"

#include "map/chunk.h"

const EventType ChunkLoaded::event_type = 0xf8b52643;

ChunkLoaded::ChunkLoaded(const Chunk* chunk)
	: chunk{ chunk }
{}
