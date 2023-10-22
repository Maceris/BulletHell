#include "MeshData.h"

void MeshData::append_vertices_to_buffer(std::vector<float>& buffer)
{
	const size_t data_size = vertices.size() 
		* sizeof(MeshVertex) / sizeof(float);
	const float* data_start = reinterpret_cast<float*>(vertices.data());
	const float* data_end = data_start + data_size;

	buffer.insert(buffer.end(), data_start, data_end);
}

void MeshData::append_weights_to_buffer(std::vector<float>& buffer)
{
	const size_t data_size = bone_weights.size() 
		* sizeof(BoneWeights) / sizeof(float);
	const float* data_start = reinterpret_cast<float*>(bone_weights.data());
	const float* data_end = data_start + data_size;

	buffer.insert(buffer.end(), data_start, data_end);
}
