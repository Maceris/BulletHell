#pragma once

#include "Globals.h"

#include <memory>

#include "vec2.hpp"

#include "Entities/EntityTypes.h"
#include "Graphics/Scene/Entity.h"

/// <summary>
/// Some kind of entity that is controllable by the player or AI.
/// </summary>
struct Pawn
{
	/// <summary>
	/// The scene enitity that this pawn is tracking, so that we can move 
	/// it around or trigger an animation.
	/// </summary>
	std::shared_ptr<Entity> scene_entity;

	/// <summary>
	/// The current health of this pawn.
	/// </summary>
	Health health;

	/// <summary>
	/// The maximum health of the pawn, so we can tell what percentage of 
	/// health it has.
	/// </summary>
	Health max_health;

	/// <summary>
	/// The delta direction describing the direction the pawn wants to move 
	/// towards.
	/// 
	/// Represents the x and z coordinates in the world, and should be
	/// normalized if not zero.
	/// 
	/// A value of (+1, 0) would mean that the pawn wants to move in
	/// the +x direction at full speed.
	/// </summary>
	glm::vec2 desired_movement;

	/// <summary>
	/// The direction the pawn wants to face, represented as a normalized
	/// vector with x and z components. A value of (0, -1) would mean that
	/// the pawn wants to face towards the -z axis.
	/// </summary>
	glm::vec2 desired_facing;
};