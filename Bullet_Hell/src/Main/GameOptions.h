#pragma once

#include "Globals.h"

#include <string>
#include <unordered_map>

#include "Utilities/GenericIterator.h"

/// <summary>
/// The different things that players can do using inputs. Used to bind keys.
/// </summary>
enum Action
{
	CAMERA_MOVE_FORWARD,
	CAMERA_MOVE_BACKWARD,
	CAMERA_MOVE_LEFT,
	CAMERA_MOVE_RIGHT,
	CAMERA_MOVE_DOWN,
	CAMERA_MOVE_UP,
	PLAYER_MOVE_FORWARD,
	PLAYER_MOVE_BACKWARD,
	PLAYER_MOVE_LEFT,
	PLAYER_MOVE_RIGHT,
	PLAYER_ATTACK,
	PAUSE_OR_UNPAUSE_GAME
};

typedef Iterator<Action, CAMERA_MOVE_FORWARD, PAUSE_OR_UNPAUSE_GAME> ActionIterator;

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

	/// <summary>
	/// Binding from GLFW keys to which action to take when they are pressed.
	/// </summary>
	std::unordered_map<int, Action> key_bindings;
};