#include "entities/pawn_manager.h"

#include <cmath>
#include <float.h>

#include "ai/brain.h"
#include "debugging/logger.h"
#include "debugging/timer.h"
#include "entities/bullet.h"
#include "entities/pawn.h"
#include "graphics/graph/animation_resource.h"
#include "graphics/graph/model_resource.h"
#include "graphics/scene/entity.h"
#include "graphics/scene/scene.h"
#include "main/game_logic.h"
#include "utilities/math_util.h"

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
constexpr float ENEMY_MOVE_SPEED = PLAYER_MOVE_SPEED * 0.40f;

/// <summary>
/// The base movement speed of bullets, in world units per timestep.
/// </summary>
constexpr float BULLET_MOVE_SPEED = PLAYER_MOVE_SPEED * 3.0f;

/// <summary>
/// The maximum number of enemies that we can ever have at once.
/// </summary>
constexpr int MAX_ENEMIES = 1000;

/// <summary>
/// How many seconds between pawn spawns.
/// </summary>
constexpr double SECONDS_PER_SPAWN = 2.0;

/// <summary>
/// How far away from the player enemies can spawn.
/// </summary>
constexpr double SPAWN_RADIUS = 50;

/// <summary>
/// The distance from the center of a player or enemy that is considered
/// to be within the collision area of the pawn.
/// </summary>
constexpr float COLLISION_RADIUS = 0.5;

/// <summary>
/// The squared value of the collision radius, to make distance calculations
/// easier.
/// </summary>
constexpr float COLLISION_RADIUS_SQUARED = COLLISION_RADIUS 
	* COLLISION_RADIUS;

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
	player->desired_facing = glm::vec2(0.0f, 1.0f);

	auto enemy_model = load_model("models/enemy/enemy.model");
	g_game_logic->current_scene->add_model(enemy_model);
	enemy_model_id = enemy_model->id;

	enemy_attack_animation = load_animation("models/enemy/enemy.human_male_cast_unarmed_magic.animation");
	enemy_idle_animation = load_animation("models/enemy/enemy.human_male_idle.animation");
	enemy_running_animation = load_animation("models/enemy/enemy.human_male_run.animation");

	auto enemy_bullet_model = load_model("models/projectile/gem_red.model");
	g_game_logic->current_scene->add_model(enemy_bullet_model);
	enemy_bullet_model_id = enemy_bullet_model->id;

	auto player_bullet_model = load_model("models/projectile/gem_blue.model");
	g_game_logic->current_scene->add_model(player_bullet_model);
	player_bullet_model_id = player_bullet_model->id;

	std::random_device random_device;
	random.seed(random_device());
}

void PawnManager::fire_enemy_bullet(Pawn& enemy)
{
	enemy.seconds_since_attack = 0;

	auto bullet = std::make_shared<Entity>(enemy_bullet_model_id);
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

void PawnManager::fire_player_bullet()
{
	player->seconds_since_attack = 0;

	auto bullet = std::make_shared<Entity>(player_bullet_model_id);
	const glm::vec3 position = player->scene_entity->position;
	const glm::vec3 offset{
		player->desired_facing.x,
		3.0f,
		player->desired_facing.y
	};
	bullet->position = position + offset;
	g_game_logic->current_scene->add_entity(bullet);
	bullet->update_model_matrix();

	std::shared_ptr<Bullet> projectile = std::make_shared<Bullet>(50,
		player->desired_facing, BULLET_MOVE_SPEED, bullet);

	player_bullets.push_back(projectile);
}

void PawnManager::reset()
{
	player_bullets.clear();
	enemy_bullets.clear();
	enemies.clear();
	player->max_health = 1000;
	player->health = player->max_health;
	player->desired_facing = glm::vec2(0.0f, 1.0f);
	player->desired_movement = glm::vec2(0.0f, 0.0f);
	player->scene_entity->position = glm::vec3(0);
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

	enemies.push_back(std::make_shared<Pawn>(enemy_entity, 200));
}

void PawnManager::tick()
{
	tick_ai();
	tick_attacks();
	tick_bullets();
	tick_movement();
}

void PawnManager::tick_animations()
{
	player->scene_entity->animation_data.next_frame();
	for (auto& pawn : enemies)
	{
		AnimationData& animation_data = pawn->scene_entity->animation_data;
		if (animation_data.current_animation)
		{
			animation_data.next_frame();
		}
	}
}

void inline PawnManager::tick_ai()
{
	seconds_since_enemy_spawn += SIMULATION_TIMESTEP;

	for (auto iterator = enemies.begin(); iterator != enemies.end();)
	{
		if (iterator->get()->health <= 0)
		{
			iterator = enemies.erase(iterator);
		}
		else
		{
			++iterator;
		}
	}

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

	for (auto& enemy : enemies)
	{
		enemy->seconds_since_attack += SIMULATION_TIMESTEP;
		Brain::update(*enemy, *player);
	}
	player->seconds_since_attack += SIMULATION_TIMESTEP;
}

void inline PawnManager::tick_attacks()
{
	for (auto& enemy : enemies)
	{
		if (!enemy->wants_to_attack)
		{
			continue;
		}
		if (enemy->seconds_since_attack >= TIME_BETWEEN_ENEMY_ATTACKS)
		{
			enemy->scene_entity->animation_data.
				run_immediate_once(enemy_attack_animation);
			fire_enemy_bullet(*enemy);
		}
	}
	if (player->wants_to_attack)
	{
		if (player->seconds_since_attack >= TIME_BETWEEN_PLAYER_ATTACKS)
		{
			player->scene_entity->animation_data.
				run_immediate_once(player_attack_animation);
			player->scene_entity->animation_data.current_frame_index = 0;
			fire_player_bullet();
		}
	}
}

[[nodiscard]] bool collides(const glm::vec3& bullet_position, 
	const glm::vec3& pawn_position) noexcept
{
	const glm::vec2 bullet_position_2d{ bullet_position.x, bullet_position.z };
	const glm::vec2 pawn_position_2d{ pawn_position.x, pawn_position.z };

	const float distance2 = 
		glm::distance2(bullet_position_2d, pawn_position_2d);

	return distance2 <= COLLISION_RADIUS_SQUARED;
}

void inline PawnManager::tick_bullets()
{
	for (std::shared_ptr<Bullet> bullet : enemy_bullets)
	{
		bullet->scene_entity->position +=
			glm::vec3(bullet->direction.x, 0, bullet->direction.y) 
			* BULLET_MOVE_SPEED;
		bullet->scene_entity->update_model_matrix();
		
		if (collides(bullet->scene_entity->position,
			player->scene_entity->position)
			&& !bullet->scene_entity->dead)
		{
			player->health -= bullet->damage;
			if (player->health <= 0)
			{
				player->health = 0;
				g_game_logic->end_game();
			}
			bullet->scene_entity->dead = true;
		}
		else
		{
			bullet->lifetime -= SIMULATION_TIMESTEP;
		}
	}

	while (!enemy_bullets.empty() && enemy_bullets.front()->lifetime <= 0)
	{
		enemy_bullets.pop_front();
	}

	for (std::shared_ptr<Bullet> bullet : player_bullets)
	{
		bullet->scene_entity->position +=
			glm::vec3(bullet->direction.x, 0, bullet->direction.y)
			* BULLET_MOVE_SPEED;
		bullet->scene_entity->update_model_matrix();

		for (auto& enemy : enemies)
		{
			if (collides(bullet->scene_entity->position,
				enemy->scene_entity->position)
				&& !bullet->scene_entity->dead)
			{
				enemy->health -= bullet->damage;
				bullet->scene_entity->dead = true;
			}
		}

		bullet->lifetime -= SIMULATION_TIMESTEP;
	}

	while (!player_bullets.empty() && player_bullets.front()->lifetime <= 0)
	{
		player_bullets.pop_front();
	}
}

void inline PawnManager::tick_movement()
{

	if (player->wants_to_attack)
	{
		player->desired_facing = find_enemy_nearest_player();
	}
	else if (!MathUtil::close_enough(player->desired_movement, 0.0f, 0.0f))
	{
		player->desired_facing = player->desired_movement;
	}

	update_direction(*player);
	for (auto& pawn : enemies)
	{
		update_direction(*pawn);
	}

	glm::vec3 movement{
		player->desired_movement.x,
		0.0f,
		player->desired_movement.y
	};
	movement *= PLAYER_MOVE_SPEED;

	if (movement.x != 0 || movement.z != 0)
	{
		player->scene_entity->position += movement;
		player->needs_updating = true;
		g_game_logic->current_scene->camera.add_position(movement);
	}

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
	for (auto& pawn : enemies)
	{
		update_movement(*pawn, ENEMY_MOVE_SPEED);

		auto& animation_data = pawn->scene_entity->animation_data;
	}

	if (player->needs_updating)
	{
		player->scene_entity->update_model_matrix();
		player->needs_updating = false;
	}
	
	for (auto& pawn : enemies)
	{
		if (pawn->needs_updating)
		{
			pawn->scene_entity->update_model_matrix();
			pawn->needs_updating = false;
		}
	}
}

[[nodiscard]]
glm::vec2 PawnManager::find_enemy_nearest_player()
{
	if (enemies.empty())
	{
		if (!MathUtil::close_enough(player->desired_movement, 0.0f, 0.0f))
		{
			return player->desired_movement;
		}
		return glm::vec2{ 1.0f, 0.0f };
	}

	float nearest_distance = FLT_MAX;
	std::shared_ptr<Pawn> nearest_so_far;

	const glm::vec3 player_position = player->scene_entity->position;

	for (auto& enemy : enemies)
	{
		const float current_distance = 
			glm::distance2(player_position, enemy->scene_entity->position);
		if (current_distance < nearest_distance)
		{
			nearest_distance = current_distance;
			nearest_so_far = enemy;
		}
	}

	const glm::vec3 enemy_direction_3d =
		nearest_so_far->scene_entity->position 
		- player->scene_entity->position;
	const glm::vec2 enemy_direction{
		enemy_direction_3d.x, enemy_direction_3d.z
	};

	return glm::normalize(enemy_direction);
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
	//TODO(ches) BH-49 - Check for collision
	if (movement.x != 0 || movement.z != 0)
	{
		pawn.scene_entity->position += movement;
		pawn.needs_updating = true;
	}
}