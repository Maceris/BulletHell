#include "MaterialResource.h"

#include "Portability.h"

/// <summary>
/// What is the latest material file verison we support loading.
/// </summary>
constexpr auto MAX_SUPPORTED_MATERIAL_VERSION = 1;

unsigned int MaterialLoader::get_loaded_resource_size(char* raw_buffer,
	unsigned int raw_size)
{
	return sizeof(Material);
}

std::string MaterialLoader::get_pattern()
{
	return "*.material";
}

bool MaterialLoader::load_resource(char* raw_buffer, unsigned int raw_size,
	std::shared_ptr<ResourceHandle> handle)
{
	std::shared_ptr<MaterialExtraData> extra
		= std::shared_ptr<MaterialExtraData>(ALLOC MaterialExtraData());
	handle->set_extra(extra);

	return parse_material(extra, raw_buffer, raw_size);
}

bool MaterialLoader::parse_material(
	std::shared_ptr<MaterialExtraData> extra_data, char* raw_buffer,
	unsigned int raw_size)
{
	RawStream stream{};
	stream.data = reinterpret_cast<unsigned char*>(raw_buffer);
	stream.data_size = raw_size;

	const uint32_t magic_bytes = read_uint32(stream);
	if (magic_bytes != 0xC0DE0001)
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
