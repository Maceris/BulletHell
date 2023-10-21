#include "GameLogic.h"

#include "Logger.h"
#include "MaterialResource.h"
#include "ModelResource.h"
#include "ResourceZipFile.h"
#include "TextureResource.h"

#include <filesystem>

GameLogic* g_game_logic = nullptr;

GameLogic::GameLogic()
	: current_state{ starting_up }
	, resource_cache{ nullptr }
	, render{ nullptr }
	, current_scene{ nullptr }
{
	g_game_logic = this;
}

bool GameLogic::initialize()
{
	std::filesystem::path resource_path{ "assets.zip" };
	std::filesystem::path full_resource_path = 
		std::filesystem::canonical(resource_path);

	ResourceFile* zip_file = ALLOC ResourceZipFile(full_resource_path.string());
	resource_cache = ALLOC ResourceCache(50, zip_file);

	if (!resource_cache->init())
	{
		LOG_FATAL("Failed to initialize the resource cache. Is there enough memory?");
		return false;
	}

	resource_cache->register_loader(std::make_shared<TextureLoader>());
	resource_cache->register_loader(std::make_shared<ModelLoader>());
	resource_cache->register_loader(std::make_shared<MaterialLoader>());

	window = std::unique_ptr<Window>(ALLOC Window());
	window->initialize();

	Texture::default_texture = load_texture("textures/default_texture.image");

	render = std::make_unique<Render>(*window);
	//TODO(ches) handle all the scene stuff elsewhere, and after a menu
	current_scene = std::make_shared<Scene>(window->width, window->height);
	auto player_model = load_model("models/player/human_male.model");
	current_scene->add_model(player_model);
	auto player_entity = std::make_shared<Entity>(player_model->id);
	current_scene->add_entity(player_entity);

	auto ground_model = load_model("models/terrain/terrain.model");
	current_scene->add_model(ground_model);
	auto ground_entity = std::make_shared<Entity>(ground_model->id);
	current_scene->add_entity(ground_entity);

	current_scene->scene_lights.ambient_light.intensity = 0.5f;
	current_scene->scene_lights.ambient_light.set_color(0.3f, 0.3f, 0.3f);

	current_scene->scene_lights.directional_light.intensity = 1.0f;
	current_scene->scene_lights.directional_light.set_color(1.0f, 1.0f, 1.0f);
	current_scene->scene_lights.directional_light.
		set_direction(-0.91f, 1.0f, -0.43f);

	render->recalculate_materials(*current_scene);

	return true;
}

void GameLogic::notify_about_resize(const int width, const int height)
{
	if (current_scene)
	{
		current_scene->resize(width, height);
	}
	render->resize(width, height);
}

void GameLogic::on_close()
{
	current_state = quitting;

	window->terminate();
}

void GameLogic::request_close()
{
	if (current_state == running || current_state == starting_up)
	{
		current_state = quit_requested;
	}
}

void GameLogic::run_game()
{
	current_state = running;

	while (current_state == running)
	{
		render->render(*window, *current_scene);
		window->render();
	}
	
}
