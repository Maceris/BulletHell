#pragma once

#include "Globals.h"

#include "Entities/EntityTypes.h"

/// <summary>
/// Used to track the projectiles in the scene.
/// </summary>
struct Bullet
{
	/// <summary>
	/// How much damage this bullet will do if it collides with something
	/// it is hostile to. Player bullets hurt enemies, enemy bullets hurt 
	/// players.
	/// </summary>
	Health damage;

	/// <summary>
	/// True if this was fired by a player, otherwise false.
	/// </summary>
	bool player_owned;
};
