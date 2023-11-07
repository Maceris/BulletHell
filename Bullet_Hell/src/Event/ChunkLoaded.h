#pragma once

#include "Globals.h"

#include "Event.h"
#include "Chunk.h"

class ChunkLoaded : public BaseEvent
{
public:
	const ChunkCoordinates coordinates;
	static const EventType event_type;

	explicit ChunkLoaded(const ChunkCoordinates& coordinates);
	ChunkLoaded(const ChunkLoaded&) = delete;
	ChunkLoaded& operator=(const ChunkLoaded&) = delete;
	~ChunkLoaded() = default;

	virtual const EventType& get_event_type() const
	{
		return event_type;
	}

	virtual const char* get_name() const
	{
		return "ChunkLoaded";
	}
};