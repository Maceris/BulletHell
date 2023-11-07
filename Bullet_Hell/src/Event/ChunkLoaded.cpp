#include "ChunkLoaded.h"

const EventType ChunkLoaded::event_type = 0xf8b52643;

ChunkLoaded::ChunkLoaded(const ChunkCoordinates& coordinates)
	: coordinates{ coordinates }
{}
