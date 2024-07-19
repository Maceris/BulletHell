#include "graphics/graph/mesh_draw_data.h"

#include "graphics/scene/entity.h"

AnimMeshDrawData::AnimMeshDrawData(std::shared_ptr<Entity> entity,
	int binding_pose_offset,
	int weights_offset)
	: entity{ entity }
	, binding_pose_offset{ binding_pose_offset }
	, weights_offset{ weights_offset }
{}

MeshDrawData::MeshDrawData(const int size_in_bytes, const int material,
	const int offset, const int indices,
	const AnimMeshDrawData& animated_mesh_draw_data)
	: size_in_bytes{ size_in_bytes }
	, material{ material }
	, offset{ offset }
	, indices{ indices }
	, animated_mesh_draw_data{ animated_mesh_draw_data }
	, is_animated{ true }
{}

MeshDrawData::MeshDrawData(const int size_in_bytes, const int material,
	const int offset, const int indices)
	: size_in_bytes(size_in_bytes)
	, material{ material }
	, offset{ offset }
	, indices{ indices }
	, animated_mesh_draw_data{ animated_mesh_draw_data }
	, is_animated{ false }
{}

