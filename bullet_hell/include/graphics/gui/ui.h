#pragma once

#include "globals.h"

#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

/// <summary>
/// Methods for drawing the game UI.
/// </summary>
class UI 
{
public:

	/// <summary>
	/// Draw the game over screen.
	/// </summary>
	static void draw_game_over();

	/// <summary>
	/// Draw the in-game GUI.
	/// </summary>
	static void draw_in_game();

	/// <summary>
	/// Draw the main menu.
	/// </summary>
	static void draw_main_menu();

	/// <summary>
	/// Draw the pause menu.
	/// </summary>
	static void draw_pause_menu();

	/// <summary>
	/// Set up the UI for the first time when the application starts.
	/// </summary>
	static void first_time_setup();

	/// <summary>
	/// Process any inputs to the UI.
	/// </summary>
	static void handle_input();

private:
	static void draw_controls();
	static void draw_player_health();
	static void draw_round_timer();
};