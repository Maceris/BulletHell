#include "graphics/graph/texture_resource.h"

#include <memory>

#include "portability.h"
#include "debugging/logger.h"
#include "main/game_logic.h"
#include "graphics/frontend/texture.h"
#include "graphics/frontend/texture_loader.h"
#include "resource_cache/resource_cache.h"

size_t TextureResourceLoader::get_loaded_resource_size(char* raw_buffer,
	size_t raw_size)
{
	//NOTE(ches) once we upload the data to the GPU, we don't keep it around
	return sizeof(Texture);
}

std::string TextureResourceLoader::get_pattern()
{
	return "*.image";
}

bool TextureResourceLoader::load_resource(char* raw_buffer, size_t raw_size,
	std::shared_ptr<ResourceHandle> handle)
{
	std::shared_ptr<TextureExtraData> extra
		= std::shared_ptr<TextureExtraData>(ALLOC TextureExtraData());
	handle->set_extra(extra);

	return parse_texture(extra, raw_buffer, raw_size);
}

bool TextureResourceLoader::parse_texture(
	std::shared_ptr<TextureExtraData> extra_data, char* raw_buffer,
	size_t raw_size)
{
	uint64_t size = 0;
	RawStream stream{};
	stream.data = reinterpret_cast<unsigned char*>(raw_buffer);
	stream.data_size = raw_size;

	uint32_t width = read_uint32(stream);
	uint32_t height = read_uint32(stream);

	unsigned char* image_data = nullptr;
	read_data_array(stream, &image_data, &size);

	if (size <= 0)
	{
		LOG_WARNING("Image does not contain data");
		return false;
	}

	extra_data->texture = std::make_unique<Texture>(
		TextureLoader::load(reinterpret_cast<const char*>(image_data), 
			ImageFormat::R8G8B8A8_UINT, width, height));

	safe_delete_array(image_data);
	return true;
}
