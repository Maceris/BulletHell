#pragma once

#include <string_view>

#include "graphics/frontend/image_format.h"
#include "graphics/frontend/texture.h"

namespace TextureLoader
{
	Texture load(const char* buffer, ImageFormat format, uint32_t width,
		uint32_t height);

	Texture load(const std::string_view texture_path);
};
