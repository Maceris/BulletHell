#include "Entities/PawnManager.h"

#include <cmath>

#include "AI/Brain.h"
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

#pragma region Constants
#define SMOOTH_ROTATION 1

/// <summary>
/// The base movement speed of the player, in world units per second, which is
/// used to calculate the move speed and also bullet speed.
/// </summary>
constexpr float PLAYER_MOVE_SPEED_PER_SECOND = 5.0f;

#if SMOOTH_ROTATION
/// <summary>
/// The speed of rotation for entities, in degrees per timestep.
/// </summary>
constexpr float ENTITY_ROTATION_SPEED = 
	static_cast<float>(360.0f * 2 * SIMULATION_TIMESTEP);
#endif

/// <summary>
/// The base movement speed of the player, in world units per timestep.
/// </summary>
constexpr float PLAYER_MOVE_SPEED = 
	static_cast<float>(PLAYER_MOVE_SPEED_PER_SECOND * SIMULATION_TIMESTEP);

/// <summary>
/// The base movement speed of enemies, in world units per timestep.
/// </summary>
constexpr float ENEMY_MOVE_SPEED = 
	static_cast<float>(PLAYER_MOVE_SPEED_PER_SECOND * 0.40f *
		SIMULATION_TIMESTEP
	);

/// <summary>
/// The base movement speed of bullets, in world units per timestep.
/// </summary>
constexpr float BULLET_MOVE_SPEED = 
	static_cast<float>(PLAYER_MOVE_SPEED_PER_SECOND * 1.05f *
		SIMULATION_TIMESTEP
	);

/// <summary>
/// The maximum number of enemies that we can ever have at once.
/// </summary>
constexpr int MAX_ENEMIES = 1000;

/// <summary>
/// How many seconds between pawn spawns.
/// </summary>
constexpr double SECONDS_PER_SPAWN = 0.5;

/// <summary>
/// How far away from the player enemies can spawn.
/// </summary>
constexpr double SPAWN_RADIUS = 50;

#pragma endregion

PawnManager::PawnManager()
	: player_bullets{ 1000 }
	, enemy_bullets{ 1000 }
	, player{ std::make_shared<Pawn>() }
	, random{}
	, spawn_offset{ -SPAWN_RADIUS, SPAWN_RADIUS }
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

	auto enemy_model = load_model("models/enemy/enemy.model");
	g_game_logic->current_scene->add_model(enemy_model);
	enemy_model_id = enemy_model->id;

	enemy_attack_animation = load_animation("models/enemy/enemy.human_male_cast_unarmed_magic.animation");
	enemy_idle_animation = load_animation("models/enemy/enemy.human_male_idle.animation");
	enemy_running_animation = load_animation("models/enemy/enemy.human_male_run.animation");

	auto bullet_model = load_model("models/projectile/gem.model");
	g_game_logic->current_scene->add_model(bullet_model);
	bullet_model_id = bullet_model->id;

	std::random_device random_device;
	random.seed(random_device());
}

void PawnManager::fire_bullet(Pawn& enemy)
{
	enemy.seconds_since_attack = 0;
#if PRUNE_MODELS_WITHOUT_ENTITIES
	if (!g_game_logic->current_scene->model_map.contains(bullet_model_id))
	{
		auto bullet_model = load_model("models/projectile/gem.model");
		g_game_logic->current_scene->add_model(bullet_model);
	}
#endif
	auto bullet = std::make_shared<Entity>(bullet_model_id);
	const glm::vec3 position = enemy.scene_entity->position;
	const glm::vec3 offset{ 
		enemy.desired_facing.x,
		3.0f,
		enemy.desired_facing.y
	};
	bullet->position = position + offset;
	g_game_logic->current_scene->add_entity(bullet);
	bullet->update_model_matrix();

	std::shared_ptr<Bullet> projectile = std::make_shared<Bullet>(100,
		enemy.desired_facing, BULLET_MOVE_SPEED, bullet);

	enemy_bullets.push_back(projectile);
}

void PawnManager::spawn_enemy(const float& x, const float& z)
{
#if PRUNE_MODELS_WITHOUT_ENTITIES
	if (!g_game_logic->current_scene->model_map.contains(enemy_model_id))
	{
		auto enemy_model = load_model("models/enemy/enemy.model");
		g_game_logic->current_scene->add_model(enemy_model);
	}
#endif
	auto enemy_entity = std::make_shared<Entity>(enemy_model_id);
	g_game_logic->current_scene->add_entity(enemy_entity);
	enemy_entity->position.x = x;
	enemy_entity->position.z = z;
	enemy_entity->update_model_matrix();
	enemy_entity->animation_data.set_current_animation(enemy_idle_animation);
	enemy_entity->animation_data.current_frame_index = rand() % enemy_idle_animation->frames.size();

	enemies.emplace_back(enemy_entity, 200);
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
	seconds_since_enemy_spawn += SIMULATION_TIMESTEP;

	while (enemies.size() < MAX_ENEMIES 
		&& seconds_since_enemy_spawn >= SECONDS_PER_SPAWN)
	{
		const double x = 
			player->scene_entity->position.x + spawn_offset(random);
		const double z = 
			player->scene_entity->position.z + spawn_offset(random);
		spawn_enemy(static_cast<float>(x), static_cast<float>(z));
		seconds_since_enemy_spawn -= SECONDS_PER_SPAWN;
	}

	for (Pawn& enemy : enemies)
	{
		enemy.seconds_since_attack += SIMULATION_TIMESTEP;
		Brain::update(enemy, *player);
	}
}

void inline PawnManager::tick_bullets()
{
	for (std::shared_ptr<Bullet> bullet : enemy_bullets)
	{
		bullet->scene_entity->position +=
			glm::vec3(bullet->direction.x, 0, bullet->direction.y);
		bullet->scene_entity->update_model_matrix();

		bullet->lifetime -= SIMULATION_TIMESTEP;
	}

	while (!enemy_bullets.empty() && enemy_bullets.front()->lifetime <= 0)
	{
		enemy_bullets.pop_front();
	}
}

void inline PawnManager::tick_movement()
{
	update_direction(*player);
	for (Pawn& pawn : enemies)
	{
		update_direction(pawn);
	}

	update_movement(*player, PLAYER_MOVE_SPEED);
	if (player->desired_movement.x != 0 || player->desired_movement.y != 0)
	{
		auto& animation_data = player->scene_entity->animation_data;
		animation_data.set_current_animation(player_running_animation);
	}
	else
	{
		auto& animation_data = player->scene_entity->animation_data;
		animation_data.set_current_animation(player_idle_animation);
	}
	for (Pawn& pawn : enemies)
	{
		update_movement(pawn, ENEMY_MOVE_SPEED);

		auto& animation_data = pawn.scene_entity->animation_data;
	}

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
}

void PawnManager::update_direction(Pawn& pawn)
{
	const glm::quat target_rotation = glm::angleAxis(
		glm::radians(-MathUtil::vector_to_angle(pawn.desired_facing)), 
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
#if SMOOTH_ROTATION
	const glm::quat& current_rotation = pawn.scene_entity->rotation;
	const float dot = glm::dot(current_rotation, target_rotation);
	const float angle = 1 - dot * dot;

	if (MathUtil::close_enough(angle, 0.0f))
	{
		return;
	}

	const float max_rotation = ENTITY_ROTATION_SPEED / 180.0f;
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

void PawnManager::update_movement(Pawn& pawn, const float move_speed)
{
	glm::vec3 movement{
		pawn.desired_movement.x,
		0.0f,
		pawn.desired_movement.y
	};
	movement *= move_speed;
	//TODO(ches) Check for collision
	if (movement.x != 0 || movement.z != 0)
	{
		pawn.scene_entity->position += movement;
		pawn.needs_updating = true;
	}
}