#pragma once

#include "Globals.h"

#include <memory>

#include "vec2.hpp"

#include "AI/AIState.h"
#include "Entities/EntityTypes.h"

struct Entity;

/// <summary>
/// The number of seconds between enmey attacks.
/// </summary>
constexpr double TIME_BETWEEN_ENEMY_ATTACKS = 2.0;

/// <summary>
/// The number of seconds between player attacks.
/// </summary>
constexpr double TIME_BETWEEN_PLAYER_ATTACKS = 0.5;

/// <summary>
/// The duration of an attack in seconds.
/// </summary>
constexpr double ATTACK_DURATION = 20.0 / 24.0;

/// <summary>
/// Some kind of entity that is controllable by the player or AI.
/// </summary>
struct Pawn
{
	friend class PawnManager;
	friend class Brain;

	/// <summary>
	/// The delta direction describing the direction the pawn wants to face 
	/// towards.
	/// 
	/// Represents the x and z coordinates in the world, and should be
	/// normalized if not zero.
	/// 
	/// A value of (+1, 0) would mean that the pawn wants to face in
	/// the +x direction.
	/// </summary>
	glm::vec2 desired_facing;

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
	/// The current health of this pawn.
	/// </summary>
	Health health;

	/// <summary>
	/// The maximum health of the pawn, so we can tell what percentage of 
	/// health it has.
	/// </summary>
	Health max_health;

	/// <summary>
	/// The scene enitity that this pawn is tracking, so that we can move 
	/// it around or trigger an animation.
	/// </summary>
	std::shared_ptr<Entity> scene_entity;

	/// <summary>
	/// Indicates that the pawn wants to attack.
	/// </summary>
	bool wants_to_attack = false;

	Pawn();
	Pawn(std::shared_ptr<Entity> entity, Health health = 0);
	Pawn(const Pawn&) = default;
	Pawn& operator=(const Pawn&) = default;
	~Pawn();

private:
	/// <summary>
	/// Whether we have made an update to the corresponding scene entity.
	/// </summary>
	bool needs_updating = false;

	/// <summary>
	/// What the pawn is currently doing. Only relevant for AI controlled
	/// pawns.
	/// </summary>
	AIState state;

	/// <summary>
	/// How many seconds it has been since the enemy has attacked.
	/// </summary>
	double seconds_since_attack = 0;
};