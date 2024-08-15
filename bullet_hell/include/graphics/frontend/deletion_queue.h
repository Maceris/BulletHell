#pragma once

#include <optional>

#include "memory/concurrent_queue.h"

struct Buffer;
struct Framebuffer;
struct Texture;

class DeletionQueue
{
public:
	enum class ResourceType : uint8_t
	{
		/// <summary>
		/// Not a valid resource, should not be used outside of errors.
		/// </summary>
		INVALID,
		BUFFER,
		FRAMEBUFFER,
		TEXTURE
	};

	struct Entry
	{
		Entry()
			: type{ ResourceType::INVALID }
			, resource{ nullptr }
		{}
		Entry(ResourceType type, void* resource)
			: type{ type }
			, resource{ resource }
		{}

		Entry(const Entry&) = default;
		Entry& operator=(const Entry&) = default;
		Entry(Entry&&) = default;
		Entry& operator=(Entry&&) = default;
		~Entry() = default;

		ResourceType type;
		void* resource;
	};

	DeletionQueue();
	DeletionQueue(const DeletionQueue&) = delete;
	DeletionQueue& operator=(const DeletionQueue&) = delete;
	~DeletionQueue();

	void add(Buffer* buffer);
	void add(Framebuffer* framebuffer);
	void add(Texture* texture);

	std::optional<Entry> pop();

private:
	ConcurrentQueue<Entry> queue;
};
