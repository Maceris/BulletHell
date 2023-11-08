#include "Graphics/Graph/ModelResource.h"

#include "Debugging/Logger.h"
#include "Graphics/Graph/MaterialResource.h"
#include "Graphics/Graph/MeshData.h"
#include "Main/GameLogic.h"

/// <summary>
/// What is the latest model file verison we support loading.
/// </summary>
constexpr auto MAX_SUPPORTED_MODEL_VERSION = 1;

size_t ModelLoader::get_loaded_resource_size(char* raw_buffer,
	size_t raw_size)
{
	RawStream stream{};
	stream.data = reinterpret_cast<unsigned char*>(raw_buffer);
	stream.data_size = raw_size;

	size_t total_size = sizeof(Model);

	stream.bytes_read += 4; // magic bytes

	const uint16_t version = read_uint16(stream);
	LOG_ASSERT(version <= MAX_SUPPORTED_MODEL_VERSION
		&& "Unsupported model version");

	stream.bytes_read += 4; // max weights

	const uint64_t mesh_count = read_uint64(stream);

	total_size += mesh_count * sizeof(MeshData);
	for (uint64_t i = 0; i < mesh_count; ++i)
	{
		stream.bytes_read += 26; // material index, aabb_min, aabb_max

		const size_t vertex_bytes = read_uint64(stream) * sizeof(MeshVertex);
		stream.bytes_read += vertex_bytes;
		total_size += vertex_bytes;

		const uint64_t index_bytes = read_uint64(stream) * sizeof(uint32_t);
		stream.bytes_read += index_bytes;
		total_size += index_bytes;

		const uint64_t weight_bytes = read_uint64(stream) * sizeof(BoneWeights);
		stream.bytes_read += weight_bytes;
		total_size += weight_bytes;
	}

	LOG_ASSERT(total_size < INT_MAX);
	return (int) total_size;
}

std::string ModelLoader::get_pattern()
{
	return "*.model";
}

bool ModelLoader::load_resource(char* raw_buffer, size_t raw_size,
	std::shared_ptr<ResourceHandle> handle)
{
	std::shared_ptr<ModelExtraData> extra
		= std::shared_ptr<ModelExtraData>(ALLOC ModelExtraData());
	handle->set_extra(extra);

	return parse_model(extra, raw_buffer, raw_size, handle->get_name());
}

/// <summary>
/// Calculates the name of a material based on the model.
/// </summary>
/// <param name="model_name">The name of the model.</param>
/// <param name="material_id">The material ID.</param>
/// <returns>The name of the material.</returns>
std::string constexpr 
material_name(const std::string& model_name, const uint16_t material_id)
{
	std::string model = model_name;
	auto dot = model.find_last_of("\\.");
	const size_t extension_size = model.size() - dot;
	std::string prefix = model.erase(dot + 1, extension_size - 1);

	return prefix + std::to_string(material_id) + ".material";
}

bool ModelLoader::parse_model(std::shared_ptr<ModelExtraData> extra_data,
	char* raw_buffer, size_t raw_size, const std::string& file_name)
{
	RawStream stream{};
	stream.data = reinterpret_cast<unsigned char*>(raw_buffer);
	stream.data_size = raw_size;

	//NOTE(ches) mostly for checking endianness
	const uint32_t magic_bytes = read_uint32(stream);
	LOG_ASSERT(magic_bytes == 0xC0DE0001 
		&& "This model does not start with the expected file ID bytes");

	const uint16_t version = read_uint16(stream);
	LOG_ASSERT(version <= MAX_SUPPORTED_MODEL_VERSION
		&& "Unsupported model version");

	const uint32_t max_weights = read_uint32(stream);
	LOG_ASSERT(max_weights == MAX_WEIGHTS_PER_BONE
		&& "Unexpected max weights per bone");

	const uint64_t mesh_count = read_uint64(stream);

	extra_data->model = std::make_shared<Model>(file_name);

	for (unsigned int i = 0; i < mesh_count; ++i)
	{
		MeshData& mesh_data = extra_data->model->mesh_data_list.emplace_back();

		uint16_t material_index = read_uint16(stream);
		Resource material(material_name(file_name, material_index));
		auto material_handle 
			= g_game_logic->resource_cache->get_handle(&material);
		std::shared_ptr<MaterialExtraData> material_extra_data
			= static_pointer_cast<MaterialExtraData>(
				material_handle->get_extra()
			);
		mesh_data.material = material_extra_data->material;

		mesh_data.aabb_min = read_vec3(stream);
		mesh_data.aabb_max = read_vec3(stream);

		const uint64_t vertex_count = read_uint64(stream);

		for (unsigned int row = 0; row < vertex_count; ++row)
		{
			MeshVertex& vertex = mesh_data.vertices.emplace_back();

			//NOTE(ches) directly overwrite the memory as floats
			float* first_value = reinterpret_cast<float*>(&vertex);
			const size_t float_count = sizeof(vertex) / sizeof(float);

			uint32_t raw_value = 0;
			for (size_t offset = 0; offset < float_count; ++offset)
			{
				raw_value = read_uint32(stream);
				*(first_value + offset) = reinterpret_cast<float&>(raw_value);
			}
		}

		const uint64_t index_count = read_uint64(stream);
		for (unsigned int j = 0; j < index_count; ++j)
		{
			mesh_data.indices.emplace_back(read_uint32(stream));
		}

		const uint64_t weight_count = read_uint64(stream);
		for (unsigned int j = 0; j < weight_count; ++j)
		{
			BoneWeights& weights = mesh_data.bone_weights.emplace_back();
			uint32_t raw_value = 0;
			for (unsigned int i = 0; i < MAX_WEIGHTS_PER_BONE; ++i)
			{
				raw_value = read_uint32(stream);
				weights.weights[i] = reinterpret_cast<float&>(raw_value);
			}
			for (unsigned int i = 0; i < MAX_WEIGHTS_PER_BONE; ++i)
			{
				raw_value = read_uint32(stream);
				weights.indices[i] = reinterpret_cast<float&>(raw_value);
			}
		}
	}

	std::string model_name = file_name;
	auto dot = model_name.find_last_of("\\.");
	const size_t extension_size = model_name.size() - dot;
	std::string prefix = model_name.erase(dot + 1, extension_size - 1);
	std::string animation_filter =  prefix + "*.animation";
	extra_data->model->animation_list = 
		g_game_logic->resource_cache->match(animation_filter);

	LOG_INFO(file_name + " had "
		+ std::to_string(extra_data->model->animation_list.size())
		+ " animations");
	
	return true;
}

std::shared_ptr<Model> load_model(const std::string& name)
{
	Resource resource(name);
	auto handle = g_game_logic->resource_cache->get_handle(&resource);
	std::shared_ptr<ModelExtraData> model_extra =
		static_pointer_cast<ModelExtraData>(handle->get_extra());
	return model_extra->model;
}