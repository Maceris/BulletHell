#include "Entities/PawnManager.h"

#include "Graphics/Graph/AnimationResource.h"
#include "Graphics/Graph/ModelResource.h"
#include "Main/GameLogic.h"

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
	player->desired_facing = glm::vec2(0, 0);
}

void PawnManager::tick_ai()
{

}

void PawnManager::tick_bullets()
{

}
