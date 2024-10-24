#pragma once

#include <string>
#include <memory>
#include <vector>

#include "glm/mat4x4.hpp"

#include "graphics/graph/animation.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/graph/mesh_draw_data.h"
#include "graphics/scene/entity.h"

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
	/// A list of animation names that can be applied to the model.
	/// </summary>
	std::vector<std::shared_ptr<Animation>> animation_list;

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
	/// Whether this model is animated.
	/// </summary>
	/// <returns></returns>
	bool is_animated();

	/// <summary>
	/// Construct a new model.
	/// </summary>
	/// <param name="id">The globally unique ID of the model.</param>
	Model(const std::string id);

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;
	~Model() = default;
};