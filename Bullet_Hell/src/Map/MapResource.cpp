#include "MapResource.h"

#include "GameLogic.h"

size_t MapLoader::get_loaded_resource_size(char* raw_buffer,
	size_t raw_size)
{
	return sizeof(Map);
}

std::string MapLoader::get_pattern()
{
	return "*.tmx";
}

bool MapLoader::load_resource(char* raw_buffer, size_t raw_size,
	std::shared_ptr<ResourceHandle> handle)
{
	std::shared_ptr<MapExtraData> extra
		= std::shared_ptr<MapExtraData>(ALLOC MapExtraData());
	handle->set_extra(extra);

	return parse_map(extra, raw_buffer, raw_size);
}

bool MapLoader::parse_map(
	std::shared_ptr<MapExtraData> extra_data, char* raw_buffer,
	size_t raw_size)
{
	//TODO(ches) parse the map
	return false;
}

std::shared_ptr<Map> load_map(const std::string& name)
{
	Resource resource(name);
	auto handle = g_game_logic->resource_cache->get_handle(&resource);
	std::shared_ptr<MapExtraData> model_extra =
		static_pointer_cast<MapExtraData>(handle->get_extra());
	return model_extra->map;
}