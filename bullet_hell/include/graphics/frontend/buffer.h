#pragma once

#include <cstdint>

using BufferHandle = uint64_t;

struct Buffer
{
	enum class Type : uint8_t;

	Buffer(Type type);
	Buffer(const Buffer&) = default;
	Buffer& operator=(const Buffer&) = default;
	/// <summary>
	/// Does nothing important, the expectation is the backing resources are 
	/// cleaned up via the deletion queue.
	/// </summary>
	~Buffer() = default;

	BufferHandle handle;
	Type type;
};

enum class Buffer::Type : uint8_t
{
	UNIFORM,
	SHADER_STORAGE
};
