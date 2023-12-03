#pragma once

#include "Globals.h"

/// <summary>
/// Represents the state of an AI character.
/// </summary>
enum AIState
{
	/// <summary>
	/// Attacking the player.
	/// </summary>
	ATTACKING,
	/// <summary>
	/// Running towards the player.
	/// </summary>
	CHASING,
	/// <summary>
	/// Running away from the player.
	/// </summary>
	FLEEING,
	/// <summary>
	/// Standing around not doing much.
	/// </summary>
	IDLE
};