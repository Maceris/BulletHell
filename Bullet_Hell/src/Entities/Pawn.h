#pragma once

#include "Globals.h"

#include <memory>

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
};