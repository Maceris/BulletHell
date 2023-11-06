#pragma once

#include "Globals.h"

#include "Event.h"
#include "Chunk.h"

class ChunkLoadedInformation : public EventInformation
{
public:
	ChunkCoordinates coordinates;

	ChunkLoadedInformation(const ChunkCoordinates& coordinates);
	ChunkLoadedInformation(const ChunkLoadedInformation&) = delete;
	ChunkLoadedInformation& operator=(const ChunkLoadedInformation&) = delete;
	~ChunkLoadedInformation() = default;
};

class ChunkLoaded : public Event
{
public:
	std::shared_ptr<ChunkLoadedInformation> information;

	ChunkLoaded(const ChunkCoordinates& coordinates);
	ChunkLoaded(const ChunkLoaded&) = delete;
	ChunkLoaded& operator=(const ChunkLoaded&) = delete;
	~ChunkLoaded() = default;

	virtual std::string event_type()
	{
		return "ChunkLoaded";
	}

	virtual std::shared_ptr<EventInformation> get_information();
};