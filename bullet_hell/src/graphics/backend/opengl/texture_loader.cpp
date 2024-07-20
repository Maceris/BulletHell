#if BACKEND_CURRENT == BACKEND_OPENGL

#include "glad.h"

#include "main/game_logic.h"
#include "graphics/backend/base/format_mapper.h"
#include "graphics/frontend/texture.h"
#include "graphics/frontend/texture_loader.h"
#include "graphics/graph/texture_resource.h"
#include "resource_cache/resource_cache.h"
#include "utilities/opengl_util.h"

Texture TextureLoader::load(const char* buffer, ImageFormat format,
	uint32_t width, uint32_t height)
{
	GLuint texture_id = 0;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, FormatMapper::map(format), width, height, 0,
		FormatMapper::map(format), FormatMapper::map_type(format), buffer);
	glGenerateMipmap(GL_TEXTURE_2D);
	//1401, 0x5121
	GL_RGBA;
	GL_UNSIGNED_BYTE;
	OpenGLUtil::check_gl_errors();

	return Texture{ texture_id, width, height };
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