#pragma once

#include "Globals.h"

#include <memory>

#include "vec2.hpp"

#include "Entities/EntityTypes.h"
#include "Graphics/Scene/Entity.h"

/// <summary>
/// How long bullets last before despawning, measured in seconds.
/// </summary>
constexpr double BULLET_LIFESPAN = 3.0;

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
	glm::vec2 direction;

	/// <summary>
	/// The speed of the projectile, in world units per second.
	/// </summary>
	float speed;

	/// <summary>
	/// The scene entity.
	/// </summary>
	std::shared_ptr<Entity> scene_entity;

	/// <summary>
	/// How long the bullet has left before it despawns, measured in seconds.
	/// </summary>
	double lifetime = BULLET_LIFESPAN;

	Bullet(const Health damage, const glm::vec2& direction, const float speed, 
		std::shared_ptr<Entity> scene_entity);
	Bullet(const Bullet&) = delete;
	Bullet& operator=(const Bullet&) = delete;
	~Bullet();
};
