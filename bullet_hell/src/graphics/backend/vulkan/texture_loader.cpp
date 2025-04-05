#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include "main/game_logic.h"
#include "graphics/backend/base/format_mapper.h"
#include "graphics/frontend/texture.h"
#include "graphics/frontend/texture_loader.h"
#include "graphics/graph/texture_resource.h"
#include "resource_cache/resource_cache.h"

Texture TextureLoader::load(const char* buffer, ImageFormat format,
	uint32_t width, uint32_t height)
{
	//TODO(ches) load textures
	return Texture{ 0, width, height };
}

Texture TextureLoader::load(const std::string_view name)
{
	Resource resource(name);
	auto handle = g_game_logic->resource_cache->get_handle(&resource);
	std::shared_ptr<TextureExtraData> model_extra =
		static_pointer_cast<TextureExtraData>(handle->get_extra());
	return *(model_extra->texture);
}

#endif