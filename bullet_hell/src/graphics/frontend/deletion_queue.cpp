#include "graphics/frontend/deletion_queue.h"

#include "graphics/frontend/buffer.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/texture.h"

DeletionQueue::DeletionQueue()
{

}

DeletionQueue::~DeletionQueue()
{
	//TODO(ches) handle this
}

void DeletionQueue::add(Buffer* buffer)
{
	queue.push({ ResourceType::BUFFER , buffer });
}

void DeletionQueue::add(Framebuffer* framebuffer)
{
	queue.push({ ResourceType::FRAMEBUFFER , framebuffer });
}

void DeletionQueue::add(Texture* texture)
{
	queue.push({ ResourceType::TEXTURE, texture });
}

std::optional<DeletionQueue::Entry> DeletionQueue::pop()
{
	Entry result;

	bool had_result = queue.try_pop(result);

	if (!had_result)
	{
		return {};
	}

	return std::optional{ result };
}
