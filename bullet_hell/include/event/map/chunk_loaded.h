#pragma once

#include "globals.h"

#include "event/event.h"

struct Chunk;

/// <summary>
/// A section of the map that has been loaded and needs graphics.
/// </summary>
class ChunkLoaded : public BaseEvent
{
public:
	const Chunk* chunk;
	static const EventType event_type;

	explicit ChunkLoaded(const Chunk* chunk);
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