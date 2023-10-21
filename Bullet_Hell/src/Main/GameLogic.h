#pragma once

#include "Globals.h"

#include "GameOptions.h"
#include "ResourceCache.h"
#include "Render.h"
#include "Window.h"

#include <memory>

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

	/// <summary>
	/// The window we are showing.
	/// </summary>
	std::unique_ptr<Window> window;

	/// <summary>
	/// Handles all the rendering, other than literally swapping buffers.
	/// </summary>
	std::unique_ptr<Render> render;

	/// <summary>
	/// The scene we are currently in.
	/// </summary>
	std::shared_ptr<Scene> current_scene;

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
};

/// <summary>
/// A global reference to the game logic object.
/// </summary>
extern GameLogic* g_game_logic;
