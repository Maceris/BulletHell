#pragma once

#include "Globals.h"

#include "boost/circular_buffer.hpp"

#include "Entities/Bullet.h"
#include "Entities/Pawn.h"

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
	/// Update all of the AIs.
	/// </summary>
	void tick_ai();

	/// <summary>
	/// Update all of the bullets.
	/// </summary>
	void tick_bullets();

	boost::circular_buffer<Bullet> player_bullets;
	boost::circular_buffer<Bullet> enemy_bullets;

	std::shared_ptr<Pawn> player;
	std::vector<Pawn> enemies;
};