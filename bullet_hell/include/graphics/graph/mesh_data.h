#pragma once

#include "Globals.h"

#include <memory>
#include <vector>

#include "vec3.hpp"

#include "Graphics/Graph/Animation.h"
#include "Graphics/Graph/Material.h"

#pragma pack(push,1)
/// <summary>
/// The data for each vertex of a mesh, as passed to the graphics card.
/// </summary>
struct MeshVertex
{
	/// <summary>
	/// The x, y, and z position of the vertex.
	/// </summary>
	float position[3];
	/// <summary>
	/// The x, y, and z components of the normal vector.
	/// </summary>
	float normal[3];
	/// <summary>
	/// The x, y, and z components of the tangent vector.
	/// </summary>
	float tangent[3];
	/// <summary>
	/// The x, y, and z components of the bitangent vector.
	/// </summary>
	float bitangent[3];
	/// <summary>
	/// The u and v texture coordinates.
	/// </summary>
	float texture_coordinates[2];
};
#pragma pack(pop)

/// <summary>
/// Vertex and face information for a mesh.
/// </summary>
struct MeshData
{
	/// <summary>
	/// A tightly-packed array of vertex data for the mesh.
	/// </summary>
	std::vector<MeshVertex> vertices;

	/// <summary>
	/// The index data for the faces.
	/// </summary>
	std::vector<uint32_t> indices;

	/// <summary>
	/// The material that this mesh uses.
	/// </summary>
	std::shared_ptr<Material> material;

	/// <summary>
	/// The weights and index of each bone that affects each vertex. There 
	/// is one set of bone weights for each vertex. Only animated models have
	/// this data filled out.
	/// </summary>
	std::vector<BoneWeights> bone_weights;

	/// <summary>
	/// The minimum point of the mesh Axis-Aligned Bounding Box.
	/// </summary>
	glm::vec3 aabb_min;

	/// <summary>
	/// The maximum point of the mesh Axis-Aligned Bounding Box.
	/// </summary>
	glm::vec3 aabb_max;

	/// <summary>
	/// Append the raw vertex data to a buffer.
	/// </summary>
	/// <param name="buffer">The buffer to append to.</param>
	void append_vertices_to_buffer(std::vector<float>& buffer) const;

	/// <summary>
	/// Append the bone index/weight data to a buffer.
	/// </summary>
	/// <param name="buffer">The buffer to append to.</param>
	void append_weights_to_buffer(std::vector<float>& buffer) const;

	/// <summary>
	/// Append the vertex indices to a buffer.
	/// </summary>
	/// <param name="buffer">The buffer to append to.</param>
	void append_indices_to_buffer(std::vector<uint32_t>& buffer) const;
};



