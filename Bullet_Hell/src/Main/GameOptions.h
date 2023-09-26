#pragma once

#include "Globals.h"

#include <string>

/// <summary>
/// Options for the game.
/// </summary>
struct GameOptions
{
	/// <summary>
	/// Set up default options.
	/// </summary>
	GameOptions();

	/// <summary>
	/// Clean up the struct.
	/// </summary>
	~GameOptions();

	/// <summary>
	/// The title of the window.
	/// </summary>
	std::string window_title;
};