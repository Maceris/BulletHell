#include "main/game_logic.h"

#include <numbers>
#include <filesystem>

#include "imgui.h"

#include "entities/pawn.h"
#include "entities/pawn_manager.h"
#include "event/event_manager.h"
#include "debugging/logger.h"
#include "debugging/timer.h"
#include "graphics/window.h"
#include "graphics/backend/base/pipeline_manager.h"
#include "graphics/frontend/texture.h"
#include "graphics/frontend/texture_loader.h"
#include "graphics/graph/animation_resource.h"
#include "graphics/graph/icon_resource.h"
#include "graphics/graph/material_resource.h"
#include "graphics/graph/model_resource.h"
#include "graphics/graph/texture_resource.h"
#include "graphics/gui/ui.h"
#include "graphics/scene/scene.h"
#include "map/chunk.h"
#include "map/tile.h"
#include "resource_cache/resource_cache.h"
#include "resource_cache/resource_zip_file.h"
#include "utilities/math_util.h"

#include "glad.h"
#include "GLFW/glfw3.h"

/// <summary>
/// The delay in seconds between recentering the map.
/// </summary>
constexpr double MAP_RECENTER_DELAY = 1;

/// <summary>
/// The delay between animation frames.
/// </summary>
constexpr double ANIMATION_FRAME_TIME = 1.0 / 24;

GameLogic::GameLogic()
	: current_state{ GameState::STARTING_UP }
	, resource_cache{ nullptr }
#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED
	, render{ nullptr }
#else
	, render_instance{ nullptr }
#endif
	, current_scene{ nullptr }
	, last_animation_tick{ std::chrono::steady_clock::now() }
	, last_frame{ std::chrono::steady_clock::now() }
	, last_map_recenter{ std::chrono::steady_clock::now() }
	, window{ nullptr }
	, action_state{}
{
}

void GameLogic::end_game()
{
	current_state = GameState::GAME_OVER;
}

GameState GameLogic::get_current_state() const noexcept
{
	return current_state;
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

	resource_cache->register_loader(std::make_shared<TextureResourceLoader>());
	resource_cache->register_loader(std::make_shared<ModelLoader>());
	resource_cache->register_loader(std::make_shared<MaterialLoader>());
	resource_cache->register_loader(std::make_shared<AnimationLoader>());
	resource_cache->register_loader(std::make_shared<IconLoader>());

	g_event_manager = ALLOC EventManager();

	window = ALLOC Window();
	TIME_START("Window Init");
	window->initialize();
	TIME_END("Window Init");

	for (Action action : ActionIterator())
	{
		action_state.emplace(std::make_pair(action, false));
	}

	PipelineManager::default_texture = ALLOC Texture(
		TextureLoader::load("textures/default_texture.image"));

#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED
	render = std::make_unique<Render>(*window);
#else
	render_instance = std::make_unique<Instance>(*window);
#endif

	UI::first_time_setup();

	current_scene = std::make_shared<Scene>(window->width, window->height);

	current_scene->scene_lights.ambient_light.intensity = 0.3f;
	current_scene->scene_lights.ambient_light.set_color(0.3f, 0.3f, 0.3f);

	current_scene->scene_lights.directional_light.intensity = 0.6f;
	current_scene->scene_lights.directional_light.set_color(1.0f, 1.0f, 1.0f);
	current_scene->scene_lights.directional_light.
		set_direction(-0.91f, 1.0f, -0.43f);

	current_scene->fog.active = true;
	current_scene->fog.color = glm::vec3(0.4f, 0.4f, 0.5f);
	current_scene->fog.density = 0.02f;

	current_scene->camera.set_position(-18.0f, 20.0f, 0.0f);
	current_scene->camera.set_rotation(0.82f, 1.57f);

	TIME_START("Map Init");
	current_map = std::make_shared<GameMap>();
	g_event_manager->update();
	TIME_END("Map Init");

	g_pawn_manager = ALLOC PawnManager();

	current_scene->rebuild_model_lists();
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
#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED
	render->resize(width, height);
#else
	render_instance->resize(width, height);
#endif
}

void GameLogic::on_close()
{
	current_state = GameState::QUITTING;

	window->terminate();

	safe_delete(g_pawn_manager);
	safe_delete(g_event_manager);
	safe_delete(window);
}

void GameLogic::on_key_pressed(int key, int scancode, int action, int mods)
{
	const auto& result = options.key_bindings.find(key);
	if (result == options.key_bindings.end())
	{
		//NOTE(ches) special handling for non-bound keys could go here
		return;
	}
	const Action mapped_action = result->second;

	auto current_mapping = action_state.find(mapped_action);
	if (action == GLFW_PRESS)
	{
		current_mapping->second = true;
	}
	else if (action == GLFW_RELEASE)
	{
		current_mapping->second = false;
	}
}

void GameLogic::process_input()
{
	window->mouse_input.handle_input();

#if _DEBUG
	Camera& camera = current_scene->camera;
	const float move_speed_per_second = 5.0f;
	const float mouse_sensitivity = 0.02f;
	const float move_amount = 
		static_cast<float>(seconds_since_last_frame * move_speed_per_second);
	if (action_desired(Action::CAMERA_MOVE_FORWARD))
	{
		camera.move_forward(move_amount);
	}
	if (action_desired(Action::CAMERA_MOVE_BACKWARD))
	{
		camera.move_backward(move_amount);
	}
	if (action_desired(Action::CAMERA_MOVE_LEFT))
	{
		camera.move_left(move_amount);
	}
	if (action_desired(Action::CAMERA_MOVE_RIGHT))
	{
		camera.move_right(move_amount);
	}
	if (action_desired(Action::CAMERA_MOVE_DOWN))
	{
		camera.move_down(move_amount);
	}
	if (action_desired(Action::CAMERA_MOVE_UP))
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
#endif

	glm::vec2 movement(0.0f, 0.0f);
	if (action_desired(Action::PLAYER_MOVE_FORWARD))
	{
		movement += glm::vec2(1.0f, 0.0f);
	}
	if (action_desired(Action::PLAYER_MOVE_BACKWARD))
	{
		movement += glm::vec2(-1.0f, 0.0f);
	}
	if (action_desired(Action::PLAYER_MOVE_LEFT))
	{
		movement += glm::vec2(0.0f, -1.0f);
	}
	if (action_desired(Action::PLAYER_MOVE_RIGHT))
	{
		movement += glm::vec2(0.0f, 1.0f);
	}

	if (movement.x != 0 || movement.y != 0)
	{
		movement = glm::normalize(movement);
	}
	g_pawn_manager->player->desired_movement = movement;

	if (action_desired(Action::PLAYER_ATTACK))
	{
		g_pawn_manager->player->wants_to_attack = true;
	}
	else
	{
		g_pawn_manager->player->wants_to_attack = false;
	}

	if (action_desired(Action::PAUSE_OR_UNPAUSE_GAME))
	{
		//NOTE(ches) immediately reset it to false to prevent spamming
		auto current_mapping = action_state.find(Action::PAUSE_OR_UNPAUSE_GAME);
		current_mapping->second = false;
		if (current_state == GameState::RUNNING)
		{
			on_pause();
		}
		else if (current_state == GameState::PAUSED)
		{
			on_resume();
		}
	}
}

void GameLogic::request_close()
{
	current_state = GameState::QUIT_REQUESTED;
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

[[nodiscard]]
bool GameLogic::action_desired(const Action& action) const noexcept
{
	return action_state.find(action)->second;
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
	current_state = GameState::MENU;
	TIME_START("Last Frame");//NOTE(ches) so we have this available for FPS
	while (current_state != GameState::QUIT_REQUESTED)
	{
		TIME_START("Processing Input");
		process_input();
		TIME_END("Processing Input");

		switch (current_state)
		{
		case GameState::GAME_OVER:
		case GameState::MENU:
		case GameState::PAUSED:
#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED
			render->render_just_ui(*window, *current_scene);
#elif BACKEND_CURRENT == BACKEND_OPENGL
			render_instance->render(*current_scene);
#endif
			window->render();
			break;
		case GameState::RUNNING:
			main_processing();
			break;
		case GameState::STARTING_UP:
			LOG_ERROR("Somehow starting up during main loop");
			break;
		case GameState::QUIT_REQUESTED:
			break;
		case GameState::QUITTING:
			LOG_ERROR("Somehow quitting during main loop");
		}
		
	}
}

void GameLogic::main_processing()
{
	calculate_delta_time();

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
	TIME_START("Updating Scene - Pruning Models");
	current_scene->prune_models();
	TIME_END("Updating Scene - Pruning Models");
	if (current_scene->dirty)
	{
		TIME_START("Updating Scene - Updating Model Lists");
		current_scene->rebuild_model_lists();
		TIME_END("Updating Scene - Updating Model Lists");
		TIME_START("Updating Scene - Updating Data");
#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED
		render->setup_all_data(*current_scene);
#else
		render_instance->setup_data(*current_scene);
#endif
		TIME_END("Updating Scene - Updating Data");
	}
	TIME_END("Updating Scene");

	if (seconds_since_last_animation_tick >= ANIMATION_FRAME_TIME)
	{
		last_animation_tick = std::chrono::steady_clock::now();
		seconds_since_last_animation_tick = 0;
		g_pawn_manager->tick_animations();
	}
#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED
	render->render(*window, *current_scene);
#else
	render_instance->render(*current_scene);
#endif
	window->render();
	++frame_count;
}

void GameLogic::on_pause()
{
	current_state = GameState::PAUSED;
	//TODO(ches) BH-53 - swap rendering pipelines
}

void GameLogic::on_resume()
{
	seconds_since_last_frame = 0;
	simulation_accumulator = 0;
	frame_count = 0;
	seconds_since_last_FPS_calcualation = 0;
	last_FPS = 0;
	seconds_since_last_animation_tick = 0;

	const Instant now = std::chrono::steady_clock::now();
	last_frame = now;
	last_animation_tick = now;
	last_map_recenter = now;

	current_state = GameState::RUNNING;
	//TODO(ches) BH-53 - swap rendering pipelines
}

void GameLogic::reset()
{
	for (auto& iterator : action_state)
	{
		iterator.second = false;
	}

	//NOTE(ches) finish processing anything that had been happening
	g_event_manager->update();
	current_map->reset();
	g_pawn_manager->reset();
	current_scene->reset();

	seconds_since_last_frame = 0;
	simulation_accumulator = 0;
	frame_count = 0;
	seconds_since_last_FPS_calcualation = 0;
	last_FPS = 0;
	seconds_since_last_animation_tick = 0;

	const Instant now = std::chrono::steady_clock::now();
	last_frame = now;
	last_animation_tick = now;
	last_map_recenter = now;

	round_timer = 0;

	current_scene->camera.set_position(-18.0f, 20.0f, 0.0f);
	current_scene->camera.set_rotation(0.82f, 1.57f);

	//NOTE(ches) Process all the map loading stuff
	g_event_manager->update();
	current_state = GameState::RUNNING;
}
