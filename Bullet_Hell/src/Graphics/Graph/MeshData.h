#pragma once

#include "Globals.h"

#include "glm.hpp"

#include <vector>

struct MeshData
{
	glm::vec3 aabb_max;
	glm::vec3 aabb_min;
	std::vector<float> bitangents;
	std::vector<int> bone_indices;
	int material_index;
	std::vector<float> normals;
	std::vector<float> positions;
	std::vector<float> tangents;
};