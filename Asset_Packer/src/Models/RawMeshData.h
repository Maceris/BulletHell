#pragma once

#include "glm.hpp"

#include <vector>

#include "Animation.h"

/// <summary>
/// The maximum number of weights that can affect a vertex.
/// </summary>
constexpr int VERTEX_MAX_WEIGHTS = 4;

/// <summary>
/// Raw mesh data that is loaded from a file, after some processing.
/// </summary>
struct RawMeshData
{
	/// <summary>
	/// A list of bitangent values, in x, y, z order. Bitangents point in the
	/// direction of the positive Y texture axis.
	/// </summary>
	std::vector<float> bitangents;

	/// <summary>
	/// The indices of all the faces.
	/// </summary>
	std::vector<uint32_t> indices;

	/// <summary>
	/// Lists of bone IDs that affect the vertices, and their weights, 
	/// ordered by vertex ID. Each vertex has RawMeshData::MAX_WEIGHTS entries
	/// in the list, and if there are less than that number of bones
	/// affecting the vertex, the remaining positions are filled with zeroes.
	/// </summary>
	std::vector<BoneWeights> bone_indices;

	/// <summary>
	/// A mesh uses only a single material, so if an imported model uses
	/// multiple materials, the import splits up the mesh.
	/// </summary>
	int material_id;

	/// <summary>
	/// A list of normal values, in x, y, z order.
	/// </summary>
	std::vector<float> normals;

	/// <summary>
	/// A list of vertex coordinates, in x, y, z order.
	/// </summary>
	std::vector<float> positions;

	/// <summary>
	/// A list of tangent values, in x, y, z order. Tangents point in the 
	/// direction of the positive X texture axis.
	/// </summary>
	std::vector<float> tangents;

	/// <summary>
	/// A list of texture coordinates, in u, v order.
	/// </summary>
	std::vector<float> texture_coordinates;

	/// <summary>
	/// The minimum point of the mesh AABB.
	/// </summary>
	glm::vec3 aabb_min;

	/// <summary>
	/// The maximum point of the mesh AABB.
	/// </summary>
	glm::vec3 aabb_max;

	RawMeshData()
		: material_id{ 0 }
		, aabb_min{ 0 }
		, aabb_max{ 0 }
	{}
	RawMeshData(const RawMeshData&) = default;
	RawMeshData& operator=(const RawMeshData&) = default;
	~RawMeshData() = default;
};