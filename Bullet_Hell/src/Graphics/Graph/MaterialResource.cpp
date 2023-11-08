#include "Graphics/Graph/MaterialResource.h"

#include "Debugging/Logger.h"
#include "Main/GameLogic.h"
#include "Utilities/Portability.h"

/// <summary>
/// What is the latest material file verison we support loading.
/// </summary>
constexpr auto MAX_SUPPORTED_MATERIAL_VERSION = 1;

size_t MaterialLoader::get_loaded_resource_size(char* raw_buffer,
	size_t raw_size)
{
	return sizeof(Material);
}

std::string MaterialLoader::get_pattern()
{
	return "*.material";
}

bool MaterialLoader::load_resource(char* raw_buffer, size_t raw_size,
	std::shared_ptr<ResourceHandle> handle)
{
	std::shared_ptr<MaterialExtraData> extra
		= std::shared_ptr<MaterialExtraData>(ALLOC MaterialExtraData());
	handle->set_extra(extra);

	return parse_material(extra, raw_buffer, raw_size);
}

bool MaterialLoader::parse_material(
	std::shared_ptr<MaterialExtraData> extra_data, char* raw_buffer,
	size_t raw_size)
{
	RawStream stream{};
	stream.data = reinterpret_cast<unsigned char*>(raw_buffer);
	stream.data_size = raw_size;

	const uint32_t magic_bytes = read_uint32(stream);
	if (magic_bytes != 0xC0DE0002)
	{
		LOG_ERROR("This material does not start with the expected file ID bytes");
		return false;
	}

	const uint16_t version = read_uint16(stream);
	
	if (version > MAX_SUPPORTED_MATERIAL_VERSION)
	{
		LOG_ERROR("Unsupported material version " + std::to_string(version));
		return false;
	}

	extra_data->material = std::make_shared<Material>();
	
	extra_data->material->ambient_color = read_vec4(stream);
	extra_data->material->diffuse_color = read_vec4(stream);
	extra_data->material->specular_color = read_vec4(stream);

	uint32_t reflectance_raw = read_uint32(stream);
	extra_data->material->reflectance 
		= *reinterpret_cast<float*>(&reflectance_raw);

	extra_data->material->texture_name = read_string(stream);
	extra_data->material->normal_map_name = read_string(stream);
	
	return true;
}

std::shared_ptr<Material> load_material(const std::string& name)
{
	Resource resource(name);
	auto handle = g_game_logic->resource_cache->get_handle(&resource);
	std::shared_ptr<MaterialExtraData> model_extra =
		static_pointer_cast<MaterialExtraData>(handle->get_extra());
	return model_extra->material;
}