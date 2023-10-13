#pragma once

#include "Globals.h"

#include "mat4x4.hpp"

#include "Animation.h"
#include "Entity.h"
#include "MeshData.h"
#include "MeshDrawData.h"

#include <string>
#include <memory>
#include <vector>

/// <summary>
/// A list of pointers to entities.
/// </summary>
using EntityList = std::vector<std::shared_ptr<Entity>>;

/// <summary>
/// A 3D model that can be rendered.
/// </summary>
struct Model
{
	/// <summary>
	/// The globally unique ID of the model.
	/// </summary>
	const std::string id;
	
	/// <summary>
	/// A list of animations that can be applied to the model.
	/// </summary>
	std::vector<Animation> animation_list;

	/// <summary>
	/// A list of entities that use this model.
	/// </summary>
	EntityList entity_list;

	/// <summary>
	/// A list of mesh data for this model. Each represents one mesh, since
	/// a model may have multiple.
	/// </summary>
	std::vector<MeshData> mesh_data_list;

	/// <summary>
	/// A list of mesh draw data for this model.
	/// </summary>
	std::vector<MeshDrawData> mesh_draw_data_list;

	/// <summary>
	/// The weights and index of each bone that affects each vertex. There 
	/// is one set of bone weights for each vertex. Only animated models have
	/// this data filled out.
	/// </summary>
	const std::vector<BoneWeights> bone_weights;

	/// <summary>
	/// Whether this model is animated.
	/// </summary>
	/// <returns></returns>
	bool is_animated();

	/// <summary>
	/// Construct a new model.
	/// </summary>
	/// <param name="id">The globally unique ID of the model.</param>
	/// <param name="mesh_data_list">A list of mesh data for this model.
	/// </param>
	/// <param name="animation_list">A list of animations that can be
	/// applied to the model.</param>
	Model(const std::string id, std::vector<MeshData> mesh_data_list,
		std::vector<Animation> animation_list);


	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;
	~Model() = default;

	/// <summary>
	/// Append the bone index/weight data to a buffer.
	/// </summary>
	/// <param name="buffer">The buffer to append to.</param>
	void constexpr append_weights_to_buffer(std::vector<float>& buffer)
	{
		const int data_size = (int)(
			bone_weights.size() * sizeof(BoneWeights) / sizeof(float));
		const float* data_start = (float*)bone_weights.data();
		const float* data_end = data_start + data_size;

		buffer.insert(buffer.end(), data_start, data_end);
	}
};