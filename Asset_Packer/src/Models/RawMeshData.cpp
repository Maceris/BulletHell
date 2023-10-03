#include "RawMeshData.h"

RawMeshData::RawMeshData(std::vector<float> positions,
	std::vector<float> normals, std::vector<float> tangents,
	std::vector<float> bitangents, std::vector<float> texture_coordinates,
	std::vector<int> indices, std::vector<int> bone_indices,
	std::vector<float> weights)
	: positions(std::move(positions))
	, normals(std::move(normals))
	, tangents(std::move(tangents))
	, bitangents(std::move(bitangents))
	, texture_coordinates(std::move(texture_coordinates))
	, indices(std::move(indices))
	, bone_indices(std::move(bone_indices))
	, weights(std::move(weights))
	, material_id(0)
{}