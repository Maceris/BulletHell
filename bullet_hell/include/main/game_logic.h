#pragma once

#include "globals.h"

#include <chrono>
#include <map>
#include <memory>

#include "main/game_options.h"
#include "map/game_map.h"
#include "graphics/render/render.h"
#include "graphics/scene/scene.h"

class EventManager;
class PawnManager;
class ResourceCache;
class Window;

/// <summary>
/// Where we are in the program lifecycle.
/// </summary>
enum class GameState
{
	/// <summary>
	/// The player has lost the game.
	/// </summary>
	GAME_OVER,
	/// <summary>
	/// We are currently in a menu.
	/// </summary>
	MENU,
	/// <summary>
	/// The game is currently paused and not simulating game logic.
	/// </summary>
	PAUSED,
	/// <summary>
	/// The game is not done initializing.
	/// </summary>
	STARTING_UP,
	/// <summary>
	/// The game has loaded fully and is running in the main loop.
	/// </summary>
	RUNNING,
	/// <summary>
	/// The game should shut down, but has not yet done so.
	/// </summary>
	QUIT_REQUESTED,
	/// <summary>
	/// The game is in the process of shutting down.
	/// </summary>
	QUITTING
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
	friend class UI;

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
	/// Signal that the player has died and the game is over.
	/// </summary>
	void end_game();
	
	/// <summary>
	/// Fetch the current state.
	/// </summary>
	/// <returns>The current game state.</returns>
	GameState get_current_state() const noexcept;

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
	/// Handle a key being pressed by mapping it to actions to be processed
	/// later. May do some immediate handling in edge cases.
	/// </summary>
	/// <param name="key">The keyboard key that was pressed or released.
	/// </param>
	/// <param name="scancode">The system-specific scancode of the key.</param>
	/// <param name="action">GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.</param>
	/// <param name="mods">Bit field describing which modifier keys were
	void on_key_pressed(int key, int scancode, int action, int mods);

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

	/// <summary>
	/// A mapping from actions to booleans representing whether the player is
	/// currently trying to do that action based on inputs.
	/// </summary>
	std::map<Action, bool> action_state;

	Instant last_frame;
	double seconds_since_last_frame = 0;
	double simulation_accumulator = 0;

	/// <summary>
	/// How many frames since we last checked FPS.
	/// </summary>
	int frame_count = 0;
	double seconds_since_last_FPS_calcualation = 0;
	int last_FPS = 0;

	Instant last_animation_tick;
	double seconds_since_last_animation_tick = 0;
	Instant last_map_recenter;

	[[nodiscard]] bool action_desired(const Action& action) const noexcept;

	/// <summary>
	/// Check if we need to recenter the map, and do so if required.
	/// </summary>
	void attempt_map_recenter();

	/// <summary>
	/// Calculate ellapsed time since last frame and update our internal
	/// timers.
	/// </summary>
	void calculate_delta_time();

	/// <summary>
	/// Handle the processing for one frame during normal gameplay.
	/// </summary>
	void main_processing();

	/// <summary>
	/// Handle input from the player.
	/// </summary>
	void process_input();

	/// <summary>
	/// Called to handle anything we have to do in order to put the game on
	/// hold.
	/// </summary>
	void on_pause();

	/// <summary>
	/// Called when we are resuming, to start back up with simulating and
	/// rendering.
	/// </summary>
	void on_resume();

	/// <summary>
	/// Called when we are resetting the game as if we just started playing for
	/// the first time since opening the application.
	/// </summary>
	void reset();

};

/// <summary>
/// A global reference to the game logic.
/// </summary>
extern GameLogic* g_game_logic;
