#pragma once

#include "Globals.h"

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
	static void draw_player_health();
	static void draw_round_timer();
};