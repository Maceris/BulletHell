#include "MeshData.h"

void MeshData::append_vertices_to_buffer(std::vector<float>& buffer)
{
	const int data_size = vertices.size() * sizeof(MeshVertex);
	const float* data_start = (float*)vertices.data();
	const float* data_end = data_start + data_size;

	buffer.insert(buffer.end(), data_start, data_end);
}
