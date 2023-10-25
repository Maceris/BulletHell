#include "GameLogic.h"

#include "Logger.h"
#include "MaterialResource.h"
#include "ModelResource.h"
#include "ResourceZipFile.h"
#include "TextureResource.h"
#include "Timer.h"

#include <filesystem>

GameLogic* g_game_logic = nullptr;

GameLogic::GameLogic()
	: current_state{ starting_up }
	, resource_cache{ nullptr }
	, render{ nullptr }
	, current_scene{ nullptr }
	, last_frame{ std::chrono::steady_clock::now() }
	, seconds_since_last_frame{ 0 }
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
	player_entity->scale = 0.01f;
	player_entity->update_model_matrix();
	current_scene->player = player_entity;

	auto ground_model = load_model("models/terrain/terrain.model");
	current_scene->add_model(ground_model);
	auto ground_entity = std::make_shared<Entity>(ground_model->id);
	current_scene->add_entity(ground_entity);
	ground_entity->scale = 100.0f;
	ground_entity->set_position(0, -1, 0);
	ground_entity->update_model_matrix();

	current_scene->scene_lights.ambient_light.intensity = 0.5f;
	current_scene->scene_lights.ambient_light.set_color(0.3f, 0.3f, 0.3f);

	current_scene->scene_lights.directional_light.intensity = 1.0f;
	current_scene->scene_lights.directional_light.set_color(1.0f, 1.0f, 1.0f);
	current_scene->scene_lights.directional_light.
		set_direction(-0.91f, 1.0f, -0.43f);

	current_scene->camera.set_position(-11.0f, 11.0f, 0.0f);
	current_scene->camera.add_rotation(0.42f, 1.92f);

	render->recalculate_materials(*current_scene);
	render->setup_data(*current_scene);

	return true;
}

void GameLogic::notify_about_resize(const int width, const int height)
{
	window->width = width;
	window->height = height;
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

void GameLogic::process_input()
{
	Camera& camera = current_scene->camera;
	const float move_speed_per_second = 5.0f;
	const float move_amount = 
		static_cast<float>(seconds_since_last_frame * move_speed_per_second);
	if (window->is_key_pressed(GLFW_KEY_W))
	{
		camera.move_forward(move_amount);
	}
	if (window->is_key_pressed(GLFW_KEY_S))
	{
		camera.move_backward(move_amount);
	}
	if (window->is_key_pressed(GLFW_KEY_A))
	{
		camera.move_left(move_amount);
	}
	if (window->is_key_pressed(GLFW_KEY_D))
	{
		camera.move_right(move_amount);
	}
	if (window->is_key_pressed(GLFW_KEY_LEFT_SHIFT))
	{
		camera.move_down(move_amount);
	}
	if (window->is_key_pressed(GLFW_KEY_SPACE))
	{
		camera.move_up(move_amount);
	}

}

void GameLogic::request_close()
{
	if (current_state == running || current_state == starting_up)
	{
		current_state = quit_requested;
	}
}

void GameLogic::calculate_delta_time()
{
	Instant current_frame = std::chrono::steady_clock::now();
	long long elapsed = std::chrono::duration_cast<std::chrono::microseconds>(
		current_frame - last_frame).count();
	seconds_since_last_frame = elapsed / 1'000'000.0;
	last_frame = current_frame;
}

void GameLogic::run_game()
{
	current_state = running;
	TIME_START("Last Frame");//NOTE(ches) so we have this available for FPS
	while (current_state == running)
	{
		calculate_delta_time();
		process_input();
		render->render(*window, *current_scene);
		window->render();
	}
	
}
