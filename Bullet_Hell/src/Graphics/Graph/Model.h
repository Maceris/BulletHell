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
	/// A list of mesh data for this model.
	/// </summary>
	std::vector<MeshData> mesh_data_list;

	/// <summary>
	/// A list of mesh draw data for this model.
	/// </summary>
	std::vector<MeshDrawData> mesh_draw_data_list;

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

	/// <summary>
	/// We cannot copy models.
	/// </summary>
	Model(const Model&) = delete;

	/// <summary>
	/// We cannot copy models.
	/// </summary>
	Model& operator=(const Model&) = delete;

	/// <summary>
	/// Clean up the model.
	/// </summary>
	~Model() = default;
};