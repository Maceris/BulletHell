#pragma once

#include "EventManager.h"

#include "ChunkLoaded.h"

namespace EventHandlers
{
	void register_handlers();

	void chunk_loaded(ChunkLoaded& event);
};
