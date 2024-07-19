#include "ai/brain.h"

#include "glm/glm.hpp"

#include "ai/ai_state.h"
#include "entities/pawn.h"
#include "entities/pawn_manager.h"
#include "graphics/scene/entity.h"
#include "utilities/math_util.h"

/// <summary>
/// The distance to the player that is close enough enemies will flee.
/// </summary>
constexpr double flee_radius = 10.0;

/// <summary>
/// The distance to the player that is far enough enemies will chase.
/// </summary>
constexpr double chase_radius = 20.0f;

constexpr double flee_radius_squared = flee_radius * flee_radius;
constexpr double chase_radius_squared = chase_radius * chase_radius;

void Brain::update(Pawn& enemy, const Pawn& player)
{
	const glm::vec3 player_direction_3d =
		player.scene_entity->position - enemy.scene_entity->position;
	const glm::vec2 player_direction{
		player_direction_3d.x, player_direction_3d.z
	};
	const double distance_to_player_squared = glm::length2(player_direction);

	if (distance_to_player_squared <= flee_radius_squared)
	{
		enemy.state = AIState::FLEEING;
	}
	else if (distance_to_player_squared >= chase_radius_squared)
	{
		enemy.state = AIState::CHASING;
	}
	else
	{
		if (enemy.seconds_since_attack >= TIME_BETWEEN_ENEMY_ATTACKS)
		{
			enemy.state = AIState::ATTACKING;
		}
		else
		{
			enemy.state = AIState::IDLE;
		}
	}

	switch (enemy.state)
	{
	case AIState::ATTACKING:
	case AIState::CHASING:
		enemy.desired_facing = glm::normalize(player_direction);
		break;
	case AIState::IDLE:
	default:
		break;
	case AIState::FLEEING:
		enemy.desired_facing = -glm::normalize(player_direction);
	}

	switch (enemy.state)
	{
	case AIState::ATTACKING:
		enemy.desired_movement = glm::vec2(0, 0);
		enemy.wants_to_attack = true;
		break;
	case AIState::CHASING:
		enemy.desired_movement = glm::normalize(player_direction);
		enemy.wants_to_attack = false;
		enemy.scene_entity->animation_data.
			set_current_animation(g_pawn_manager->enemy_running_animation);
		break;
	case AIState::FLEEING:
		enemy.desired_movement = -glm::normalize(player_direction);
		enemy.wants_to_attack = false;
		enemy.scene_entity->animation_data.
			set_current_animation(g_pawn_manager->enemy_running_animation);
		break;
	case AIState::IDLE:
	default:
		enemy.desired_movement = glm::vec2(0, 0);
		enemy.wants_to_attack = false;
		enemy.scene_entity->animation_data.
			set_current_animation(g_pawn_manager->enemy_idle_animation);
		break;
	}
}
