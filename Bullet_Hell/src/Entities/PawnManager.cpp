#include "Entities/PawnManager.h"

#include <cmath>

#include "Entities/Bullet.h"
#include "Entities/Pawn.h"
#include "Graphics/Graph/AnimationResource.h"
#include "Graphics/Graph/ModelResource.h"
#include "Graphics/Scene/Entity.h"
#include "Graphics/Scene/Scene.h"
#include "Main/GameLogic.h"

PawnManager* g_pawn_manager = nullptr;

/// <summary>
/// The base movement speed of entities, in world units per second, which is
/// used to calculate the move speed and also bullet speed.
/// </summary>
constexpr float entity_move_speed_per_second = 1.0f;

/// <summary>
/// The speed of rotation for entities, in degrees per timestep.
/// </summary>
constexpr float entity_rotation_speed = 360.0f * simulation_timestep;

/// <summary>
/// The base movement speed of entities, in world units per timestep.
/// </summary>
constexpr float entity_move_speed = 
	entity_move_speed_per_second * simulation_timestep;

/// <summary>
/// The base movement speed of bullets, in world units per timestep.
/// </summary>
constexpr float bullet_move_speed = 
	(entity_move_speed_per_second * 1.05f) * simulation_timestep;

PawnManager::PawnManager()
	: player_bullets{ 1000 }
	, enemy_bullets{ 1000 }
	, player{ std::make_shared<Pawn>() }
{
	auto player_model = load_model("models/player/human_male.model");
	g_game_logic->current_scene->add_model(player_model);
	auto player_entity = std::make_shared<Entity>(player_model->id);
	g_game_logic->current_scene->add_entity(player_entity);
	player_entity->update_model_matrix();
	auto idle = load_animation("models/player/human_male.human_malehuman_male_idle.animation");
	player_entity->animation_data.set_current_animation(idle);

	player->scene_entity = player_entity;
	player->max_health = 1000;
	player->health = player->max_health;
	player->desired_movement = glm::vec2(0, 0);
	player->desired_facing = 0.0f;
}

void PawnManager::tick()
{
	tick_ai();
	tick_bullets();
	tick_movement();
}

void inline PawnManager::tick_ai()
{

}

void inline PawnManager::tick_bullets()
{

}

void inline update_direction(const Pawn& pawn)
{
	const float current_rotation 
		= glm::degrees(glm::eulerAngles(pawn.scene_entity->rotation).y);

	const float actual_distance = 
		std::abs(pawn.desired_facing - current_rotation);

	if (actual_distance < 0.01)
	{
		return;
	}

	float amount_to_rotate = std::min(actual_distance, entity_rotation_speed);

	const bool rotate_counter_clockwise = std::fmodf(
		(pawn.desired_facing - current_rotation + 360.0f), 360.0f) > 180.0f;

	if (rotate_counter_clockwise)
	{
		amount_to_rotate = -amount_to_rotate;
	}

	pawn.scene_entity->add_rotation(0.0f, 1.0f, 0.0f, amount_to_rotate);
}

void inline PawnManager::tick_movement()
{
	update_direction(*player);
	for (const Pawn& pawn : enemies)
	{
		update_direction(pawn);
	}

}