#pragma once

#include "globals.h"

#include "event/event.h"
#include "map/chunk_coordinates.h"

struct Chunk;

/// <summary>
/// A region of the map that is being partially unloaded, and needs models
/// unloaded.
/// </summary>
class ChunkUnloaded : public BaseEvent
{
public:
	/// <summary>
	/// A pointer to the chunk, which should be checked before we use it just
	/// in case it fully unloaded since this event was fired.
	/// </summary>
	const Chunk* chunk;
	/// <summary>
	/// The coordinates, in case the chunk is no longer valid.
	/// </summary>
	const ChunkCoordinates coordinates;
	static const EventType event_type;

	/// <summary>
	/// Create a new event.
	/// </summary>
	/// <param name="coordinates">The coordinates, in case the chunk is no
	/// longer valid.</param>
	/// <param name="chunk">A pointer to the chunk, which should be
	/// checked before we use it just in case it fully unloaded since this
	/// event was fired.</param>
	explicit ChunkUnloaded(const ChunkCoordinates coordinates, 
		const Chunk* chunk);
	ChunkUnloaded(const ChunkUnloaded&) = delete;
	ChunkUnloaded& operator=(const ChunkUnloaded&) = delete;
	~ChunkUnloaded() = default;

	virtual const EventType& get_event_type() const
	{
		return event_type;
	}

	virtual const char* get_name() const
	{
		return "ChunkUnloaded";
	}
};