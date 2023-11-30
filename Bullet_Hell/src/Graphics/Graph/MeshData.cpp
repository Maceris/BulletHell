#include "Graphics/Graph/MeshData.h"

#include "Graphics/Graph/Animation.h"

void MeshData::append_vertices_to_buffer(std::vector<float>& buffer) const
{
	const size_t data_size = vertices.size() 
		* sizeof(MeshVertex) / sizeof(float);
	const float* data_start = reinterpret_cast<const float*>(vertices.data());
	const float* data_end = data_start + data_size;

	buffer.insert(buffer.end(), data_start, data_end);
}

void MeshData::append_weights_to_buffer(std::vector<float>& buffer) const
{
	const size_t data_size = bone_weights.size() 
		* sizeof(BoneWeights) / sizeof(float);
	const float* data_start = reinterpret_cast<const float*>(bone_weights.data());
	const float* data_end = data_start + data_size;

	buffer.insert(buffer.end(), data_start, data_end);
}

void MeshData::append_indices_to_buffer(std::vector<uint32_t>& buffer) const
{
	const size_t data_size = indices.size();
	const uint32_t* data_start = indices.data();
	const uint32_t* data_end = data_start + data_size;
	buffer.insert(buffer.end(), data_start, data_end);
}
