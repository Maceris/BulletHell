#pragma once

#include "Globals.h"

#include "glm.hpp"

#include "Entities/EntityTypes.h"

/// <summary>
/// Used to track the projectiles in the scene.
/// </summary>
struct Bullet
{
	/// <summary>
	/// How much damage this bullet will do if it collides with something
	/// it is hostile to.
	/// </summary>
	Health damage;

	/// <summary>
	/// The direction the projectile is moving in.
	/// </summary>
	glm::vec3 direction;

	/// <summary>
	/// The speed of the projectile.
	/// </summary>
	glm::vec3 speed;
};
