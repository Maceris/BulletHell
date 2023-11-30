#include "Entities/PawnManager.h"

#include <cmath>

#include "Debugging/Logger.h"
#include "Debugging/Timer.h"
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

/// <summary>
/// The base movement speed of entities, in world units per second, which is
/// used to calculate the move speed and also bullet speed.
/// </summary>
constexpr float entity_move_speed_per_second = 5.0f;

#if SMOOTH_ROTATION
/// <summary>
/// The speed of rotation for entities, in degrees per timestep.
/// </summary>
constexpr float entity_rotation_speed = 
	static_cast<float>(360.0f * 2 * simulation_timestep);
#endif

/// <summary>
/// The base movement speed of entities, in world units per timestep.
/// </summary>
constexpr float entity_move_speed = 
	static_cast<float>(entity_move_speed_per_second * simulation_timestep);

/// <summary>
/// The base movement speed of bullets, in world units per timestep.
/// </summary>
constexpr float bullet_move_speed = 
	static_cast<float>(entity_move_speed_per_second * 1.05f * 
		simulation_timestep
	);

/// <summary>
/// The maximum number of enemies that we can ever have at once.
/// </summary>
constexpr int MAX_ENEMIES = 1000;

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
	player_attack_animation = load_animation("models/player/human_male.human_male_cast_unarmed_magic.animation");
	player_idle_animation = load_animation("models/player/human_male.human_male_idle.animation");
	player_running_animation = load_animation("models/player/human_male.human_male_run.animation");
	player_entity->animation_data.set_current_animation(player_idle_animation);

	player->scene_entity = player_entity;
	player->max_health = 1000;
	player->health = player->max_health;
	player->desired_movement = glm::vec2(0, 0);
	player->desired_facing = 0.0f;

	auto enemy_model = load_model("models/enemy/enemy.model");
	g_game_logic->current_scene->add_model(enemy_model);
	enemy_model_id = enemy_model->id;

	enemy_attack_animation = load_animation("models/enemy/enemy.human_male_cast_unarmed_magic.animation");
	enemy_idle_animation = load_animation("models/enemy/enemy.human_male_idle.animation");
	enemy_running_animation = load_animation("models/enemy/enemy.human_male_run.animation");
}

void PawnManager::tick()
{
	tick_ai();
	tick_bullets();
	tick_movement();
}

void PawnManager::tick_animations()
{
	player->scene_entity->animation_data.next_frame();
	for (Pawn& pawn : enemies)
	{
		AnimationData& animation_data = pawn.scene_entity->animation_data;
		if (animation_data.current_animation)
		{
			animation_data.next_frame();
		}
	}
}

void inline PawnManager::tick_ai()
{
	const glm::vec3& player_position = player->scene_entity->position;
	for (Pawn& pawn : enemies)
	{
		const glm::vec3& pawn_position = pawn.scene_entity->position;
		const glm::vec3 player_direction = player_position - pawn_position;
		const float angle = MathUtil::vector_to_angle(
			glm::vec2(player_direction.x, player_direction.z)
		);
		pawn.desired_facing = angle;
	}
}

void inline PawnManager::tick_bullets()
{

}

void inline PawnManager::tick_movement()
{
	TIME_START("Updating direction");
	update_direction(*player);
	for (Pawn& pawn : enemies)
	{
		update_direction(pawn);
	}
	TIME_END("Updating direction");

	TIME_START("Updating movement");

	update_movement(*player);
	if (player->desired_movement.x != 0 || player->desired_movement.y != 0)
	{
		auto& animation_data = player->scene_entity->animation_data;
		if (animation_data.current_animation != player_running_animation)
		{
			animation_data.set_current_animation(player_running_animation);
		}
	}
	else
	{
		auto& animation_data = player->scene_entity->animation_data;
		if (animation_data.current_animation != player_idle_animation)
		{
			animation_data.set_current_animation(player_idle_animation);
		}
	}
	for (Pawn& pawn : enemies)
	{
		update_movement(pawn);
	}
	TIME_END("Updating movement");

	TIME_START("Updating matrices");

	if (player->needs_updating)
	{
		player->scene_entity->update_model_matrix();
		player->needs_updating = false;
	}
	
	for (Pawn& pawn : enemies)
	{
		if (pawn.needs_updating)
		{
			pawn.scene_entity->update_model_matrix();
			pawn.needs_updating = false;
		}
	}
	TIME_END("Updating matrices");

}

void PawnManager::spawn_enemy(const float& x, const float& z)
{
	auto enemy_entity = std::make_shared<Entity>(enemy_model_id);
	g_game_logic->current_scene->add_entity(enemy_entity);
	enemy_entity->position.x = x;
	enemy_entity->position.z = z;
	enemy_entity->update_model_matrix();
	enemy_entity->animation_data.set_current_animation(enemy_idle_animation);
	enemy_entity->animation_data.current_frame_index = rand() % enemy_idle_animation->frames.size();

	enemies.emplace_back(enemy_entity, 200);
}

void PawnManager::update_direction(Pawn& pawn)
{
	const glm::quat target_rotation = glm::angleAxis(
		glm::radians(-pawn.desired_facing), glm::vec3(0.0f, 1.0f, 0.0f)
	);
#if SMOOTH_ROTATION
	const glm::quat& current_rotation = pawn.scene_entity->rotation;
	const float dot = glm::dot(current_rotation, target_rotation);
	const float angle = 1 - dot * dot;

	if (MathUtil::close_enough(angle, 0.0f))
	{
		return;
	}

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
	pawn.needs_updating = true;
}

void PawnManager::update_movement(Pawn& pawn)
{
	glm::vec3 movement{
		pawn.desired_movement.x,
		0.0f,
		pawn.desired_movement.y
	};
	movement *= entity_move_speed;
	//TODO(ches) Check for collision
	if (movement.x != 0 || movement.z != 0)
	{
		pawn.scene_entity->position += movement;
		pawn.needs_updating = true;
	}
}