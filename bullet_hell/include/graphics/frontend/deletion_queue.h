#pragma once

#include <optional>

#include "memory/concurrent_queue.h"

struct Buffer;
struct Framebuffer;
struct Shader;
struct Texture;

class DeletionQueue
{
public:
	enum class ResourceType;
	struct Entry;

	void add(Buffer buffer);
	void add(Framebuffer buffer);
	void add(Shader buffer);
	void add(Texture buffer);

	std::optional<Entry> pop();

private:
	ConcurrentQueue<Entry> queue;
};

enum class DeletionQueue::ResourceType : uint8_t
{
	BUFFER,
	FRAMEBUFFER,
	SHADER,
	TEXTURE
};

struct DeletionQueue::Entry
{
	ResourceType type;
	void* resource;
};
