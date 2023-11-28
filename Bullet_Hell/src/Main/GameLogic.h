#pragma once

#include "Globals.h"

#include <chrono>
#include <memory>

#include "Main/GameOptions.h"
#include "Map/GameMap.h"
#include "Graphics/Render/Render.h"
#include "Graphics/Scene/Scene.h"

class EventManager;
class PawnManager;
class ResourceCache;
class Window;

/// <summary>
/// Where we are in the program lifecycle.
/// </summary>
enum GameState
{
	/// <summary>
	/// The game is not done initializing.
	/// </summary>
	starting_up,
	/// <summary>
	/// The game has loaded fully and is running in the main loop.
	/// </summary>
	running,
	/// <summary>
	/// The game should shut down, but has not yet done so.
	/// </summary>
	quit_requested,
	/// <summary>
	/// The game is in the process of shutting down.
	/// </summary>
	quitting
};

using Instant = std::chrono::steady_clock::time_point;

/// <summary>
/// Handles the lifecycle for the game.
/// </summary>
class GameLogic
{
#if DEBUG
	friend class DebugUI;
#endif

public:
	GameLogic();
	GameLogic(const GameLogic&) = delete;
	GameLogic& operator=(const GameLogic&) = delete;

	/// <summary>
	/// The resource cache for the game.
	/// </summary>
	ResourceCache* resource_cache;

	/// <summary>
	/// Settings for the game.
	/// </summary>
	GameOptions options;

	/// <summary>
	/// The window we are showing.
	/// </summary>
	Window* window;
	
	/// <summary>
	/// Set up data before anything loads.
	/// </summary>
	/// <returns>Whether we successfully initialized.</returns>
	bool initialize();
	
	/// <summary>
	/// Called after the window has been resized.
	/// </summary>
	/// <param name="width">The new width of the window, in pixels.</param>
	/// <param name="height">The new height of the window, in pixels.</param>
	void notify_about_resize(const int width, const int height);

	/// <summary>
	/// Called while the game is supposed to close, releases all the game 
	/// systems in reverse order from which they were created.
	/// </summary>
	void on_close();

	/// <summary>
	/// Called when we need to shut down.
	/// </summary>
	void request_close();

	/// <summary>
	/// Start running the main game logic. This will not return until the game
	/// is essentially finished shutting down.
	/// </summary>
	void run_game();

	/// <summary>
	/// Handles all the rendering, other than literally swapping buffers.
	/// </summary>
	std::unique_ptr<Render> render;

	std::shared_ptr<Scene> current_scene;
	std::shared_ptr<GameMap> current_map;

	/// <summary>
	/// The number of seconds that the current round has been going on. Counts
	/// up from 0.
	/// </summary>
	double round_timer = 0;

private:
	/// <summary>
	/// The current state of the game.
	/// </summary>
	GameState current_state;

	void process_input();

	Instant last_frame;
	double seconds_since_last_frame = 0;

	/// <summary>
	/// How many frames since we last checked FPS.
	/// </summary>
	int frame_count = 0;
	double seconds_since_last_FPS_calcualation = 0;
	int last_FPS = 0;

	Instant last_animation_tick;
	double seconds_since_last_animation_tick = 0;
	Instant last_map_recenter;

	void calculate_delta_time();

	/// <summary>
	/// Check if we need to recenter the map, and do so if required.
	/// </summary>
	void attempt_map_recenter();
};

/// <summary>
/// A global reference to the game logic.
/// </summary>
extern GameLogic* g_game_logic;
