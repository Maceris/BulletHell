#pragma once

#include "Globals.h"

#include <vector>

#include "boost/circular_buffer.hpp"

#include "Entities/Bullet.h"
#include "Entities/Pawn.h"
#include "Graphics/Graph/Animation.h"

/// <summary>
/// How long the fixed timestep is for the pawn manager, in seconds.
/// </summary>
constexpr double simulation_timestep = 1.0f / 60.0f;

/// <summary>
/// Tracks all the players, bullets, and enemies.
/// </summary>
class PawnManager
{
public:

	PawnManager();
	PawnManager(const PawnManager&) = delete;
	PawnManager& operator=(const PawnManager&) = delete;
	~PawnManager() = default;

	/// <summary>
	/// Update everything the pawn manager cares about.
	/// </summary>
	void tick();

	boost::circular_buffer<Bullet> player_bullets;
	boost::circular_buffer<Bullet> enemy_bullets;

	std::shared_ptr<Pawn> player;
	std::vector<Pawn> enemies;

private:
	/// <summary>
	/// Update all of the AIs, deciding if they want to move, attack, look
	/// in a different direction, stop, etc.
	/// </summary>
	void inline tick_ai();

	/// <summary>
	/// Update all of the bullets.
	/// </summary>
	void inline tick_bullets();

	/// <summary>
	/// Move the entities that want to do so.
	/// </summary>
	void inline tick_movement();

	std::shared_ptr<Animation> player_idle_animation;
	std::shared_ptr<Animation> player_running_animation;
};

/// <summary>
/// A global reference to the pawn manager.
/// </summary>
extern PawnManager* g_pawn_manager;
