#pragma once

#include "Globals.h"

#include <string>

#include "mat4x4.hpp"
#include "vec3.hpp"
#include "ext/quaternion_float.hpp"
#include "gtx/quaternion.hpp"

#include "Graphics/Scene/AnimationData.h"

/// <summary>
/// Something that is part of the 3D scene.
/// </summary>
struct Entity
{
	/// <summary>
	/// A globally unique ID for this entity. If there are multiple 
	/// quintillion different entities, we have larger problems than 
	/// ID collisions.
	/// </summary>
	const uint64_t entity_ID;

	/// <summary>
	/// The next valid ID to use for an entity.
	/// </summary>
	static std::atomic<uint64_t> next_ID;

	/// <summary>
	/// The ID of the model associated to this entity.
	/// </summary>
	const std::string model_ID;

	/// <summary>
	/// Animation data associated with this entity.
	/// </summary>
	AnimationData animation_data;

	/// <summary>
	/// The combined translation, rotation, and scale transformations.
	/// </summary>
	glm::mat4 model_matrix;

	/// <summary>
	/// The transformation matrix.
	/// </summary>
	glm::vec3 position;

	/// <summary>
	/// The rotation, as a quaternion to prevent gimbal lock.
	/// </summary>
	glm::quat rotation;
	
	/// <summary>
	/// The scale factor.
	/// </summary>
	float scale;

	/// <summary>
	/// Whether the scene entity should be removed.
	/// </summary>
	bool dead = false;

	/// <summary>
	/// Construct an entity.
	/// </summary>
	/// <param name="model_ID">The ID of the modal associated with this
	/// entity.</param>
	Entity(const std::string model_ID);

	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;
	~Entity() = default;

	/// <summary>
	/// Add to the current rotation. The result will be as if the current
	/// rotation was applied, then the new one was applied on top of it.
	/// </summary>
	/// <param name="x">The x component of the rotation axis.</param>
	/// <param name="y">The Y component of the rotation axis.</param>
	/// <param name="z">The z component of the rotation axis.</param>
	/// <param name="angle">The angle in degrees.</param>
	void add_rotation(const float x, const float y, const float z,
		const float angle);

	/// <summary>
	/// Set the position.
	/// </summary>
	/// <param name="x">The new x position.</param>
	/// <param name="y">The new y position.</param>
	/// <param name="z">The new z position.</param>
	void set_position(const float x, const float y, const float z);

	/// <summary>
	/// Set the rotation.
	/// </summary>
	/// <param name="x">The x component of the rotation axis.</param>
	/// <param name="y">The Y component of the rotation axis.</param>
	/// <param name="z">The z component of the rotation axis.</param>
	/// <param name="angle">The angle in degrees.</param>
	void set_rotation(const float x, const float y, const float z, 
		const float angle);

	void set_rotation(const glm::quat& new_rotation);

	/// <summary>
	/// Update the model matrix based on the current position, rotation, and 
	/// scale. To be called after making updates, after all updates are done.
	/// </summary>
	void update_model_matrix();
};