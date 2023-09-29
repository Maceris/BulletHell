#pragma once

#include "Globals.h"

#include "glm.hpp"

#include <vector>

#include "Material.h"


/// <summary>
/// The maximum number of weights that can affect a vertex.
/// </summary>
extern const int VERTEX_MAX_WEIGHTS;

/// <summary>
/// Raw mesh data that is loaded from a file, after some processing.
/// </summary>
struct MeshData
{
	/// <summary>
	/// The maximum value (corner) or the axis-aligned bounding box.
	/// </summary>
	glm::vec3 aabb_max;

	/// <summary>
	/// The minimum value (corner) or the axis-aligned bounding box.
	/// </summary>
	glm::vec3 aabb_min;

	/// <summary>
	/// A list of bitangent values, in x, y, z order. Bitangents point in the
	/// direction of the positive Y texture axis.
	/// </summary>
	std::vector<float> bitangents;

	/// <summary>
	/// The indices of all the faces.
	/// </summary>
	std::vector<int> indices;

	/// <summary>
	/// Lists of bone IDs that affect the vertices, ordered by vertex ID. Each
	/// vertex has MeshData::MAX_WEIGHTS entries in the list, and if there are
	/// less than that number of bones affecting the vertex, the remaining
	/// positions are filled with zeroes. Each index in this list is the bone
	/// ID corresponding to the weight in the weight list at the same position.
	/// </summary>
	std::vector<int> bone_indices;

	/// <summary>
	/// A mesh uses only a single material, so if an imported model uses
	/// multiple materials, the import splits up the mesh. This value is used
	/// reference the appropriate material in the material cache.
	/// </summary>
	material_id material_id;

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
	/// Lists of weights that affect the vertices, ordered by vertex ID. 
	/// Each vertex has MeshData::MAX_WEIGHTS entries in the list, and if there
	/// are less than that number of bones affecting the vertex, the remaining
	/// positions are filled with zeroes. Each index in this list is the
	/// weight corresponding to the bone ID in the bone ID list at the same
	/// position.
	/// </summary>
	std::vector<float> weights;

	/// <summary>
	/// Create mesh data.
	/// </summary>
	/// <param name="positions">A list of tangent values, in x, y, z order.
	/// </param>
	/// <param name="normals">A list of vertex coordinates, in x, y, z order.
	/// </param>
	/// <param name="tangents">A list of tangent values, in x, y, z order. Tangents
	/// point in the direction of the positive X texture axis.</param>
	/// <param name="bitangents">A list of bitangent values, in x, y, z order.
	/// Bitangents point in the direction of the positive Y texture
	/// axis.</param>
	/// <param name="texture_coordinates">A list of texture coordinates, 
	/// in x, y order.</param>
	/// <param name="indices">The indices of all the faces.</param>
	/// <param name="bone_indices">Lists of bone IDs that affect the vertices,
	/// ordered by vertex ID. Each vertex has MAX_WEIGHTS entries in the list.
	/// </param>
	/// <param name="weights">Lists of weights that affect the vertices,
	/// ordered by vertex ID. Each vertex has MAX_WEIGHTS entries in the list.
	/// </param>
	/// <param name="aabb_min">The minimum value (corner) or the axis-aligned
	/// bounding box.</param>
	/// <param name="aabb_max">The maximum value (corner) or the axis-aligned
	/// bounding box.</param>
	MeshData(std::vector<float> positions, std::vector<float> normals, 
		std::vector<float> tangents, std::vector<float> bitangents,
		std::vector<float> texture_coordinates, std::vector<int> indices,
		std::vector<int> bone_indices, std::vector<float> weights, 
		glm::vec3 aabb_min, glm::vec3 aabb_max);

	/// <summary>
	/// We don't copy the mesh data.
	/// </summary>
	MeshData(const MeshData&) = default;

	/// <summary>
	/// We don't copy the mesh data.
	/// </summary>
	MeshData& operator=(const MeshData&) = default;

	/// <summary>
	/// Clean up the data.
	/// </summary>
	~MeshData() = default;
};