#include "GameLogic.h"

#include <filesystem>

#include "glad.h"
#include "glfw3.h"
#include "imgui.h"

#include "Debugging/Logger.h"
#include "Debugging/Timer.h"
#include "Graphics/Graph/AnimationResource.h"
#include "Graphics/Graph/MaterialResource.h"
#include "Graphics/Graph/ModelResource.h"
#include "Graphics/Graph/TextureResource.h"
#include "ResourceCache/ResourceZipFile.h"

GameLogic* g_game_logic = nullptr;
EventManager* g_event_manager = nullptr;
PawnManager* g_pawn_manager = nullptr;

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
	resource_cache->register_loader(std::make_shared<AnimationLoader>());

	g_event_manager = ALLOC EventManager();

	window = std::unique_ptr<Window>(ALLOC Window());
	TIME_START("Window Init");
	window->initialize();
	TIME_END("Window Init");

	Texture::default_texture = load_texture("textures/default_texture.image");

	render = std::make_unique<Render>(*window);
	//TODO(ches) handle all the scene stuff elsewhere, and after a menu
	current_scene = std::make_shared<Scene>(window->width, window->height);

	current_scene->scene_lights.ambient_light.intensity = 0.1f;
	current_scene->scene_lights.ambient_light.set_color(0.3f, 0.3f, 0.3f);

	current_scene->scene_lights.directional_light.intensity = 0.5f;
	current_scene->scene_lights.directional_light.set_color(1.0f, 1.0f, 1.0f);
	current_scene->scene_lights.directional_light.
		set_direction(-0.91f, 1.0f, -0.43f);

	PointLight::Attenuation attenuation(0.0f, 0.0f, 1.0f);
	PointLight p1(glm::vec3(1.0f, 0.1f, 0.1f), glm::vec3(10.0f, 0.1f, 10.0f), 1.0f, attenuation);
	PointLight p2(glm::vec3(0.1f, 1.0f, 0.1f), glm::vec3(0.0f, 1.0f, -10.0f), 1.0f, attenuation);
	PointLight p3(glm::vec3(0.1f, 0.1f, 1.0f), glm::vec3(-7.0f, 1.0f, 7.0f), 1.0f, attenuation);
	current_scene->scene_lights.point_lights.push_back(p1);
	current_scene->scene_lights.point_lights.push_back(p2);
	current_scene->scene_lights.point_lights.push_back(p3);
	
	PointLight p4(glm::vec3(1.0f, 0.1f, 1.0f), glm::vec3(-13.0f, 1.0f, 27.0f), 1.0f, attenuation);
	SpotLight s1(p4, glm::vec3(-3.0f, -1.0f, 0.0f), 30.0f);
	current_scene->scene_lights.spot_lights.push_back(s1);

	current_scene->fog.active = true;
	current_scene->fog.color = glm::vec3(0.5f, 0.5f, 0.5f);
	current_scene->fog.density = 0.02f;

	current_scene->camera.set_position(-11.0f, 11.0f, 0.0f);
	current_scene->camera.add_rotation(0.42f, 1.92f);

	TIME_START("Map Init");
	current_map = std::make_shared<GameMap>();
	g_event_manager->update();
	TIME_END("Map Init");

	g_pawn_manager = ALLOC PawnManager();

	current_scene->rebuild_model_lists();
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

	SAFE_DELETE(g_pawn_manager);
	SAFE_DELETE(g_event_manager);
}

void GameLogic::process_input()
{
	window->mouse_input.handle_input();
	Camera& camera = current_scene->camera;
	const float move_speed_per_second = 5.0f;
	const float mouse_sensitivity = 0.02f;
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

	if (window->mouse_input.right_button_pressed 
		&& !ImGui::GetIO().WantCaptureMouse)
	{
		const glm::vec2& displace = window->mouse_input.displacement;
		camera.add_rotation(
			displace.x * mouse_sensitivity, 
			displace.y * mouse_sensitivity
		);
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
		TIME_START("Processing Input");
		process_input();
		TIME_END("Processing Input");

		TIME_START("Processing Events");
		g_event_manager->update(10);
		TIME_END("Processing Events");

		TIME_START("Updating Scene");
		if (current_scene->dirty)
		{
			current_scene->rebuild_model_lists();
			render->recalculate_materials(*current_scene);
			render->setup_data(*current_scene);
			current_scene->dirty = false;
		}
		TIME_END("Updating Scene");

		g_pawn_manager->player->scene_entity->animation_data.next_frame();
		render->render(*window, *current_scene);
		window->render();
	}
	
}
