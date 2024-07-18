#pragma once

#include "Globals.h"

#include <random>
#include <vector>

#include "boost/circular_buffer.hpp"

#include "Entities/Bullet.h"
#include "Entities/Pawn.h"
#include "Graphics/Graph/Animation.h"

/// <summary>
/// How long the fixed timestep is for the pawn manager, in seconds.
/// </summary>
constexpr double SIMULATION_TIMESTEP = 1.0f / 60.0f;

/// <summary>
/// Tracks all the players, bullets, and enemies.
/// </summary>
class PawnManager
{
public:
	friend class Brain;

	PawnManager();
	PawnManager(const PawnManager&) = delete;
	PawnManager& operator=(const PawnManager&) = delete;
	~PawnManager() = default;

	/// <summary>
	/// Resets as if we had just started a new game.
	/// </summary>
	void reset();

	/// <summary>
	/// Update everything the pawn manager cares about.
	/// </summary>
	void tick();

	/// <summary>
	/// Update the animations for things that the pawn manager cares about.
	/// </summary>
	void tick_animations();

	boost::circular_buffer<std::shared_ptr<Bullet>> player_bullets;
	boost::circular_buffer<std::shared_ptr<Bullet>> enemy_bullets;

	std::shared_ptr<Pawn> player;
	std::vector<std::shared_ptr<Pawn>> enemies;

private:

	std::shared_ptr<Animation> player_attack_animation;
	std::shared_ptr<Animation> player_idle_animation;
	std::shared_ptr<Animation> player_running_animation;

	std::string enemy_model_id;
	std::shared_ptr<Animation> enemy_attack_animation;
	std::shared_ptr<Animation> enemy_idle_animation;
	std::shared_ptr<Animation> enemy_running_animation;

	std::string enemy_bullet_model_id;
	std::string player_bullet_model_id;

	double seconds_since_enemy_spawn = 0;

	/// <summary>
	/// Used to generate random numbers for the manager.
	/// </summary>
	std::mt19937 random;

	/// <summary>
	/// A random distribution used for calculating offsets from the player 
	/// while spawning enemies.
	/// </summary>
	std::uniform_real_distribution<double> spawn_offset;

	/// <summary>
	/// Find the direction that the player would have to face in order to
	/// be aiming at the nearest enemy.
	/// </summary>
	/// <returns>The diretion that would be facing the nearest enemy.</returns>
	[[nodiscard]] glm::vec2 find_enemy_nearest_player();
	
	/// <summary>
	/// Fires a bullet wherever the enemy is looking.
	/// </summary>
	/// <param name="enemy">The enemy that is attacking.</param>
	void fire_enemy_bullet(Pawn& enemy);

	/// <summary>
	/// Fires a player bullet.
	/// </summary>
	void fire_player_bullet();

	/// <summary>
	/// Spawn an enemy at the specified world coordiantes.
	/// </summary>
	/// <param name="x">The x location of the enemy.</param>
	/// <param name="z">The z location of the enemy.</param>
	void spawn_enemy(const float& x, const float& z);

	/// <summary>
	/// Update all of the AIs, deciding if they want to move, attack, look
	/// in a different direction, stop, etc.
	/// </summary>
	void inline tick_ai();

	/// <summary>
	/// Execute on the desire for attacks.
	/// </summary>
	void inline tick_attacks();

	/// <summary>
	/// Update all of the bullets.
	/// </summary>
	void inline tick_bullets();

	/// <summary>
	/// Move the entities that want to do so.
	/// </summary>
	void inline tick_movement();

	/// <summary>
	/// Update the direction of the pawn based on the desired direction.
	/// </summary>
	/// <param name="pawn">The pawn to update.</param>
	void update_direction(Pawn& pawn);

	/// <summary>
	/// Update the position of a pawn based on the desired movement.
	/// </summary>
	/// <param name="pawn">The pawn to update.</param>
	void update_movement(Pawn& pawn, const float move_speed);
};

/// <summary>
/// A global reference to the pawn manager.
/// </summary>
extern PawnManager* g_pawn_manager;
