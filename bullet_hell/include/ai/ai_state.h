#pragma once

/// <summary>
/// Represents the state of an AI character.
/// </summary>
enum class AIState
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