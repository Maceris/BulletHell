#pragma once

#include <cstdint>

using BufferHandle = uint64_t;

struct Buffer
{
	enum class Type;

	BufferHandle handle;
	Type type;
};

enum class Buffer::Type : uint8_t
{
	UNIFORM,
	SHADER_STORAGE
};
