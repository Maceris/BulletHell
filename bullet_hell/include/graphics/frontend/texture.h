#pragma once

#include <cstdint>

using TextureHandle = uint64_t;

struct Texture
{
	TextureHandle handle;
	uint32_t width;
	uint32_t height;
};
