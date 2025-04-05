#pragma once

#include "graphics/frontend/backend_type.h"
#include "graphics/frontend/image_format.h"

#if BACKEND_CURRENT == BACKEND_VULKAN
#include <vulkan/vulkan.h>
#endif

namespace FormatMapper
{
#if BACKEND_CURRENT == BACKEND_OPENGL
	/// <summary>
	/// Convert the specified format into an implementation-specific format
	/// constant for the pixel data.
	/// </summary>
	/// <param name="format">The format we want to represent.</param>
	/// <returns>The internal format value.</returns>
	int map(const ImageFormat format);

	/// <summary>
	/// Convert the specified format into an implementation-specific 
	/// constant for texel type.
	/// </summary>
	/// <param name="format">The format we want to represent.</param>
	/// <returns>
	/// The constant representing the data type of texel data.
	/// </returns>
	int map_type(const ImageFormat format);

#elif BACKEND_CURRENT == BACKEND_VULKAN
	/// <summary>
	/// Convert the specified format into an implementation-specific format
	/// constant for the pixel data.
	/// </summary>
	/// <param name="format">The format we want to represent.</param>
	/// <returns>The internal format value.</returns>
	VkFormat map(const ImageFormat format);
#endif
}