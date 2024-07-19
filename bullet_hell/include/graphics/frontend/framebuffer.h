#pragma once

#include <cstdint>
#include <vector>

#include "graphics/frontend/texture.h"

using FramebufferHandle = uint64_t;

struct Framebuffer
{
	FramebufferHandle handle;
	uint32_t width;
	uint32_t height;
	std::vector<TextureHandle> textures;
};
