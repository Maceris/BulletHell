#pragma once

#include "Globals.h"

#include <memory>

#include "Entity.h"
#include "Material.h"

/// <summary>
/// Data used for drawing animated meshes.
/// </summary>
struct AnimMeshDrawData
{
	/// <summary>
	/// The entity associated with the animation.
	/// </summary>
	std::shared_ptr<Entity> entity;

	/// <summary>
	/// The offset to the binding pose within the data.
	/// </summary>
	int binding_pose_offset;

	/// <summary>
	/// The offset to the weight within the data.
	/// </summary>
	int weights_offset;

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

	/// <summary>
	/// Copy the struct.
	/// </summary>
	AnimMeshDrawData(const AnimMeshDrawData&) = default;

	/// <summary>
	/// Copy the struct.
	/// </summary>
	AnimMeshDrawData& operator=(const AnimMeshDrawData&) = default;

	/// <summary>
	/// Clean up data.
	/// </summary>
	~AnimMeshDrawData() = default;
};

struct MeshDrawData
{
	/// <summary>
	/// The size of the mesh in bytes.
	/// </summary>
	int size_in_bytes;

	/// <summary>
	/// The material ID that the mesh is associated with.
	/// </summary>
	material_id material;

	/// <summary>
	/// The offset, in rows.
	/// </summary>
	int offset;

	/// <summary>
	/// The number of indices.
	/// </summary>
	int vertices;

	/// <summary>
	/// Whether this contains animated mesh draw data. If false, the animated
	/// mesh draw data is junk and should be ignored.
	/// </summary>
	bool is_animated;

	/// <summary>
	/// The animation mesh draw data. Only valid data if is_animated is true.
	/// </summary>
	AnimMeshDrawData animated_mesh_draw_data;

	/// <summary>
	/// Set up mesh draw data for a mesh without animation draw data.
	/// </summary>
	/// <param name="size_in_bytes">The size of the mesh in bytes.</param>
	/// <param name="material">The material ID that the mesh is associated
	/// with.</param>
	/// <param name="offset">The offset, in rows.</param>
	/// <param name="vertices">The number of indices.</param>
	MeshDrawData(int size_in_bytes, material_id material, int offset,
		int vertices);

	/// <summary>
	/// Set up mesh draw data for a mesh that does have animation draw data.
	/// </summary>
	/// <param name="size_in_bytes">The size of the mesh in bytes.</param>
	/// <param name="material">The material ID that the mesh is associated
	/// with.</param>
	/// <param name="offset">The offset, in rows.</param>
	/// <param name="vertices">The number of indices.</param>
	/// <param name="animated_mesh_draw_data">The animation mesh draw data.
	/// </param>
	MeshDrawData(int size_in_bytes, material_id material, int offset,
		int vertices, AnimMeshDrawData animated_mesh_draw_data);

	/// <summary>
	/// Copy the struct.
	/// </summary>
	MeshDrawData(const MeshDrawData&) = default;

	/// <summary>
	/// Copy the struct.
	/// </summary>
	MeshDrawData& operator=(const MeshDrawData&) = default;

	/// <summary>
	/// Clean up data.
	/// </summary>
	~MeshDrawData() = default;
};

