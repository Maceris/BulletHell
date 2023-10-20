#include "MeshDrawData.h"

AnimMeshDrawData::AnimMeshDrawData(std::shared_ptr<Entity> entity,
	int binding_pose_offset,
	int weights_offset)
	: entity(std::move(entity))
	, binding_pose_offset(binding_pose_offset)
	, weights_offset(weights_offset)
{}

MeshDrawData::MeshDrawData(const int size_in_bytes, const MaterialID material,
	const int offset, const int vertices,
	const AnimMeshDrawData& animated_mesh_draw_data)
	: size_in_bytes(size_in_bytes)
	, material(material)
	, offset(offset)
	, vertices(vertices)
	, animated_mesh_draw_data(animated_mesh_draw_data)
	, is_animated(true)
{}

MeshDrawData::MeshDrawData(const int size_in_bytes, const MaterialID material,
	const int offset, const int vertices)
	: size_in_bytes(size_in_bytes)
	, material(material)
	, offset(offset)
	, vertices(vertices)
	, animated_mesh_draw_data(animated_mesh_draw_data)
	, is_animated(false)
{}

