#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include "debugging/logger.h"
#include "graphics/backend/base/format_mapper.h"

int FormatMapper::map(const ImageFormat format)
{
	return -1;
}
#endif