#include "GameLogic.h"

#include <numbers>
#include <filesystem>

#include "imgui.h"

#include "Entities/Pawn.h"
#include "Entities/PawnManager.h"
#include "Event/EventManager.h"
#include "Debugging/Logger.h"
#include "Debugging/Timer.h"
#include "Graphics/Window.h"
#include "Graphics/Graph/AnimationResource.h"
#include "Graphics/Graph/IconResource.h"
#include "Graphics/Graph/MaterialResource.h"
#include "Graphics/Graph/ModelResource.h"
#include "Graphics/Graph/Texture.h"
#include "Graphics/Graph/TextureResource.h"
#include "Graphics/GUI/UI.h"
#include "Graphics/Render/Render.h"
#include "Graphics/Scene/Scene.h"
#include "Main/GameOptions.h"
#include "Map/Chunk.h"
#include "Map/GameMap.h"
#include "Map/Tile.h"
#include "ResourceCache/ResourceCache.h"
#include "ResourceCache/ResourceZipFile.h"
#include "Utilities/MathUtil.h"

#include "glad.h"
#include "glfw3.h"

GameLogic* g_game_logic = nullptr;

/// <summary>
/// The delay in seconds between recentering the map.
/// </summary>
constexpr double MAP_RECENTER_DELAY = 1;

/// <summary>
/// The delay between animation frames.
/// </summary>
constexpr double ANIMATION_FRAME_TIME = 1.0 / 24;

GameLogic::GameLogic()
	: current_state{ starting_up }
	, resource_cache{ nullptr }
	, render{ nullptr }
	, current_scene{ nullptr }
	, last_animation_tick{ std::chrono::steady_clock::now() }
	, last_frame{ std::chrono::steady_clock::now() }
	, last_map_recenter{ std::chrono::steady_clock::now() }
	, window{ nullptr }
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
	resource_cache->register_loader(std::make_shared<IconLoader>());

	g_event_manager = ALLOC EventManager();

	window = ALLOC Window();
	TIME_START("Window Init");
	window->initialize();
	TIME_END("Window Init");

	Texture::default_texture = load_texture("textures/default_texture.image");

	render = std::make_unique<Render>(*window);

	UI::first_time_setup();

	//TODO(ches) handle all the scene stuff elsewhere, and after a menu
	current_scene = std::make_shared<Scene>(window->width, window->height);

	current_scene->scene_lights.ambient_light.intensity = 0.3f;
	current_scene->scene_lights.ambient_light.set_color(0.3f, 0.3f, 0.3f);

	current_scene->scene_lights.directional_light.intensity = 0.6f;
	current_scene->scene_lights.directional_light.set_color(1.0f, 1.0f, 1.0f);
	current_scene->scene_lights.directional_light.
		set_direction(-0.91f, 1.0f, -0.43f);

	current_scene->fog.active = false;
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
	current_scene->dirty = true;

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
	SAFE_DELETE(window);
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

	glm::vec2 movement(0.0f, 0.0f);
	if (window->is_key_pressed(GLFW_KEY_UP))
	{
		movement += glm::vec2(1.0f, 0.0f);
	}
	if (window->is_key_pressed(GLFW_KEY_DOWN))
	{
		movement += glm::vec2(-1.0f, 0.0f);
	}
	if (window->is_key_pressed(GLFW_KEY_LEFT))
	{
		movement += glm::vec2(0.0f, -1.0f);
	}
	if (window->is_key_pressed(GLFW_KEY_RIGHT))
	{
		movement += glm::vec2(0.0f, 1.0f);
	}

	if (movement.x != 0 || movement.y != 0)
	{
		movement = glm::normalize(movement);
	}
	g_pawn_manager->player->desired_movement = movement;

	float angle = 0.0f;

	if (MathUtil::close_enough(movement, 0.0f, 0.0f))
	{
		angle = g_pawn_manager->player->desired_facing;
	}
	else
	{
		angle = MathUtil::vector_to_angle(movement);
	}
	
	g_pawn_manager->player->desired_facing = angle;

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
	round_timer += seconds_since_last_frame;

	seconds_since_last_FPS_calcualation += seconds_since_last_frame;
	if (seconds_since_last_FPS_calcualation > 1.0)
	{
		last_FPS = frame_count;
		frame_count = 0;
		seconds_since_last_FPS_calcualation = 0;
	}

	seconds_since_last_animation_tick += seconds_since_last_frame;
	
}

void GameLogic::attempt_map_recenter()
{
	Instant now = std::chrono::steady_clock::now();
	long long elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(
			now - last_map_recenter
		).count();
	double seconds_since_map_recenter = elapsed / 1'000'000.0;
	if (seconds_since_map_recenter >= MAP_RECENTER_DELAY)
	{
		last_map_recenter = now;

		const glm::vec3 player_position = 
			g_pawn_manager->player->scene_entity->position;

		glm::i16vec2 tile_coordinates{
			std::floorf(player_position.x / (CHUNK_WIDTH * TILE_SCALE * 2)),
			std::floorf(player_position.z / (CHUNK_WIDTH * TILE_SCALE * 2))
		};

		ChunkCoordinates actual_coordinates{
			tile_coordinates.x,
			tile_coordinates.y
		};

		if (current_map->center != actual_coordinates)
		{
			current_map->recenter(current_map->center, actual_coordinates);
		}
	}
}

void GameLogic::run_game()
{
	double simulation_accumulator = 0.0f;
	current_state = running;
	TIME_START("Last Frame");//NOTE(ches) so we have this available for FPS
	while (current_state == running)
	{
		calculate_delta_time();
		TIME_START("Processing Input");
		process_input();
		TIME_END("Processing Input");

		TIME_START("Updating Pawns");
		simulation_accumulator += seconds_since_last_frame;
		while (simulation_accumulator >= SIMULATION_TIMESTEP)
		{
			g_pawn_manager->tick();
			simulation_accumulator -= SIMULATION_TIMESTEP;
		}
		TIME_END("Updating Pawns");

		TIME_START("Recentering Map");
		attempt_map_recenter();
		TIME_END("Recentering Map");

		TIME_START("Processing Events");
		g_event_manager->update(10);
		TIME_END("Processing Events");

		TIME_START("Updating Scene");
		if (current_scene->dirty)
		{
			current_scene->rebuild_model_lists();
			render->setup_all_data(*current_scene);
		}
		TIME_END("Updating Scene");

		if (seconds_since_last_animation_tick >= ANIMATION_FRAME_TIME)
		{
			last_animation_tick = std::chrono::steady_clock::now();
			seconds_since_last_animation_tick = 0;
			g_pawn_manager->tick_animations();
		}

		render->render(*window, *current_scene);
		window->render();
		++frame_count;
	}
	
}
