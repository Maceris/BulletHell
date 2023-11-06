#include "ChunkLoaded.h"

ChunkLoadedInformation::ChunkLoadedInformation(
	const ChunkCoordinates& coordinates)
	: coordinates{ coordinates }
{}

ChunkLoaded::ChunkLoaded(const ChunkCoordinates& coordinates)
	: information{ std::make_shared<ChunkLoadedInformation>(coordinates) }
{}

std::shared_ptr<EventInformation> ChunkLoaded::get_information()
{
	return information;
}