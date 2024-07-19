#include "graphics/graph/icon_resource.h"

#include "portability.h"
#include "debugging/logger.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

size_t IconLoader::get_loaded_resource_size(char* raw_buffer,
	size_t raw_size)
{
	return raw_size;
}

std::string IconLoader::get_pattern()
{
	return "textures/game_icon*.image";
}

bool IconLoader::load_resource(char* raw_buffer, size_t raw_size,
	std::shared_ptr<ResourceHandle> handle)
{
	std::shared_ptr<IconExtraData> extra
		= std::shared_ptr<IconExtraData>(ALLOC IconExtraData());
	handle->set_extra(extra);

	return parse_icon(extra, raw_buffer, raw_size);
}

bool IconLoader::parse_icon(std::shared_ptr<IconExtraData> extra_data,
	char* raw_buffer, size_t raw_size)
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

	extra_data->icon = std::make_shared<Icon>(width, height, image_data);

	return true;
}

std::shared_ptr<Icon> load_icon(const std::string& name)
{
	Resource resource(name);
	auto handle = g_game_logic->resource_cache->get_handle(&resource);
	std::shared_ptr<IconExtraData> model_extra =
		static_pointer_cast<IconExtraData>(handle->get_extra());
	return model_extra->icon;
}