#include "MeshData.h"

MeshData::MeshData(std::vector<float> positions, std::vector<float> normals,
	std::vector<float> tangents, std::vector<float> bitangents,
	std::vector<float> texture_coordinates, std::vector<int> indices,
	std::vector<int> bone_indices, std::vector<float> weights,
	glm::vec3 aabb_min, glm::vec3 aabb_max)
	: positions(std::move(positions))
	, normals(std::move(normals))
	, tangents(std::move(tangents))
	, bitangents(std::move(bitangents))
	, texture_coordinates(std::move(texture_coordinates))
	, indices(std::move(indices))
	, bone_indices(std::move(bone_indices))
	, weights(std::move(weights))
	, aabb_min(std::move(aabb_min))
	, aabb_max(std::move(aabb_max))
	, material_id(0)
{}