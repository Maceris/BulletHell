#pragma once

#include <cstdint>
#include <vector>

#include "graphics/frontend/texture.h"

using FramebufferHandle = uint64_t;

struct Framebuffer
{
	Framebuffer(FramebufferHandle handle, uint32_t width, uint32_t height,
		const std::vector<TextureHandle>& textures)
		: handle{ handle }
		, width{ width }
		, height{ height }
		, textures{ textures }
	{}
	Framebuffer(const Framebuffer&) = default;
	Framebuffer& operator=(const Framebuffer&) = default;

	/// <summary>
	/// Does nothing important, the expectation is the backing resources are 
	/// cleaned up via the deletion queue.
	/// </summary>
	~Framebuffer() = default;

	FramebufferHandle handle;
	uint32_t width;
	uint32_t height;
	std::vector<TextureHandle> textures;
};
