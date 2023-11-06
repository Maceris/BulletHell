#include "EventHandlers.h"

#include "GameLogic.h"
#include "Logger.h"

void EventHandlers::register_handlers()
{
	g_event_manager->register_handler("ChunkLoaded", 
		EventHandler::create([](Event& event)
			{
				chunk_loaded(static_cast<ChunkLoaded&>(event)); 
			}
		)
	);
}

void EventHandlers::chunk_loaded(ChunkLoaded& event)
{
	//TODO(ches) handle loading models for all the tiles in the chunk
	LOG_INFO("We are doing something with a loaded chunk");
}