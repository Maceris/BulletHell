#pragma once

#include "Globals.h"

/// <summary>
/// Methods for drawing the game UI.
/// </summary>
class UI 
{
public:
	/// <summary>
	/// Draw all of the UI elements for the game.
	/// </summary>
	static void draw();

	/// <summary>
	/// Process any inputs to the UI.
	/// </summary>
	static void handle_input();

private:
	static void draw_player_health();
};