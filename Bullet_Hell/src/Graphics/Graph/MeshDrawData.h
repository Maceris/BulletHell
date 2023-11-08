#pragma once

#include "Globals.h"

#include <memory>

#include "Graphics/Graph/Material.h"
#include "Graphics/Scene/Entity.h"

/// <summary>
/// Data used for drawing animated meshes.
/// </summary>
struct AnimMeshDrawData
{
	/// <summary>
	/// The entity associated with the animation.
	/// </summary>
	const std::shared_ptr<Entity> entity;

	/// <summary>
	/// The offset to the binding pose within the data.
	/// </summary>
	const int binding_pose_offset;

	/// <summary>
	/// The offset to the weight within the data.
	/// </summary>
	const int weights_offset;

	/// <summary>
	/// Set up new draw data.
	/// </summary>
	/// <param name="entity">The entity associated with the animation.</param>
	/// <param name="binding_pose_offset">The offset to the binding pose
	/// within the data.</param>
	/// <param name="weights_offset">The offset to the weight within the data.
	/// </param>
	AnimMeshDrawData(std::shared_ptr<Entity> entity, int binding_pose_offset,
		int weights_offset);

	AnimMeshDrawData(const AnimMeshDrawData&) = default;
	AnimMeshDrawData& operator=(const AnimMeshDrawData&) = default;
	~AnimMeshDrawData() = default;
};

struct MeshDrawData
{
	/// <summary>
	/// The size of the mesh in bytes.
	/// </summary>
	const int size_in_bytes;

	/// <summary>
	/// The material ID that the mesh is associated with.
	/// </summary>
	const MaterialID material;

	/// <summary>
	/// The offset, in rows.
	/// </summary>
	const int offset;

	/// <summary>
	/// The number of indices.
	/// </summary>
	const int indices;

	/// <summary>
	/// Whether this contains animated mesh draw data. If false, the animated
	/// mesh draw data is junk and should be ignored.
	/// </summary>
	const bool is_animated;

	/// <summary>
	/// The animation mesh draw data. Only valid data if is_animated is true.
	/// </summary>
	const AnimMeshDrawData animated_mesh_draw_data;

	/// <summary>
	/// Set up mesh draw data for a mesh without animation draw data.
	/// </summary>
	/// <param name="size_in_bytes">The size of the mesh in bytes.</param>
	/// <param name="material">The material ID that the mesh is associated
	/// with.</param>
	/// <param name="offset">The offset to the base vertex.</param>
	/// <param name="indices">The number of indices.</param>
	MeshDrawData(const int size_in_bytes, const MaterialID material,
		const int offset, const int indices);

	/// <summary>
	/// Set up mesh draw data for a mesh that does have animation draw data.
	/// </summary>
	/// <param name="size_in_bytes">The size of the mesh in bytes.</param>
	/// <param name="material">The material ID that the mesh is associated
	/// with.</param>
	/// <param name="offset">The offset to the base vertex.</param>
	/// <param name="indices">The number of indices.</param>
	/// <param name="animated_mesh_draw_data">The animation mesh draw data.
	/// </param>
	MeshDrawData(const int size_in_bytes, const MaterialID material,
		const int offset, const int indices, 
		const AnimMeshDrawData& animated_mesh_draw_data);

	MeshDrawData(const MeshDrawData&) = default;
	MeshDrawData& operator=(const MeshDrawData&) = default;
	~MeshDrawData() = default;
};

