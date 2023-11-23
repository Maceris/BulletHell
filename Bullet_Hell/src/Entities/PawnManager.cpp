#include "Entities/PawnManager.h"

#include <cmath>

#include "Debugging/Logger.h"
#include "Entities/Bullet.h"
#include "Entities/Pawn.h"
#include "Graphics/Graph/AnimationResource.h"
#include "Graphics/Graph/ModelResource.h"
#include "Graphics/Scene/Entity.h"
#include "Graphics/Scene/Scene.h"
#include "Main/GameLogic.h"
#include "Utilities/MathUtil.h"

PawnManager* g_pawn_manager = nullptr;

#define SMOOTH_ROTATION 1

inline float normalize_float_angle(const float& degrees)
{
	if (MathUtil::close_enough(degrees, 0)
		|| MathUtil::close_enough(degrees, 180))
	{
		return degrees;
	}
	if (degrees < 0)
	{
		return -degrees;
	}
	return 360.0f - degrees;
}

/// <summary>
/// The base movement speed of entities, in world units per second, which is
/// used to calculate the move speed and also bullet speed.
/// </summary>
constexpr float entity_move_speed_per_second = 1.0f;

#if SMOOTH_ROTATION
/// <summary>
/// The speed of rotation for entities, in degrees per timestep.
/// </summary>
constexpr float entity_rotation_speed = 360.0f * 2 * simulation_timestep;
#endif

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
	const glm::quat target_rotation = glm::angleAxis(
		glm::radians(-pawn.desired_facing), glm::vec3(0.0f, 1.0f, 0.0f)
	);
#if SMOOTH_ROTATION
	const glm::quat& current_rotation = pawn.scene_entity->rotation;
	const float dot = glm::dot(current_rotation, target_rotation);
	const float angle = 1 - dot * dot;

	const float max_rotation = entity_rotation_speed / 180.0f;
	if (angle > max_rotation)
	{
		const float percentage = max_rotation / angle;
		const glm::quat& result = 
			glm::slerp(current_rotation, target_rotation, percentage);
		pawn.scene_entity->set_rotation(result);
	}
	else
	{
		pawn.scene_entity->set_rotation(target_rotation);
	}
#else
	pawn.scene_entity->set_rotation(target_rotation);
#endif
	pawn.scene_entity->update_model_matrix();
}

void inline PawnManager::tick_movement()
{
	update_direction(*player);
	for (const Pawn& pawn : enemies)
	{
		update_direction(pawn);
	}

}