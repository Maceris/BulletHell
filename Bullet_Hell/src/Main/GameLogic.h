#pragma once

#include "Globals.h"

#include <chrono>
#include <memory>

#include "Event/EventManager.h"
#include "Graphics/Render/Render.h"
#include "Graphics/Window.h"
#include "Main/GameOptions.h"
#include "Map/GameMap.h"
#include "ResourceCache/ResourceCache.h"

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
private:
	/// <summary>
	/// The current state of the game.
	/// </summary>
	GameState current_state;

	void process_input();

	Instant last_frame;

	double seconds_since_last_frame;

	void calculate_delta_time();

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
	std::unique_ptr<Window> window;
	
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
};

/// <summary>
/// A global reference to the game logic object.
/// </summary>
extern GameLogic* g_game_logic;

/// <summary>
/// A global reference to the event manager object.
/// </summary>
extern EventManager* g_event_manager;
