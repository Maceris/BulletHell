#include "Main/GameOptions.h"

#include "glfw3.h"

GameOptions::GameOptions()
	: window_title{ "Bullet Hell" }
	, key_bindings{}
{
	key_bindings.insert(std::make_pair(GLFW_KEY_W, CAMERA_MOVE_FORWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_S, CAMERA_MOVE_BACKWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_A, CAMERA_MOVE_LEFT));
	key_bindings.insert(std::make_pair(GLFW_KEY_D, CAMERA_MOVE_RIGHT));
	key_bindings.insert(std::make_pair(GLFW_KEY_LEFT_SHIFT, CAMERA_MOVE_DOWN));
	key_bindings.insert(std::make_pair(GLFW_KEY_SPACE, CAMERA_MOVE_UP));
	key_bindings.insert(std::make_pair(GLFW_KEY_UP, PLAYER_MOVE_FORWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_DOWN, PLAYER_MOVE_BACKWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_LEFT, PLAYER_MOVE_LEFT));
	key_bindings.insert(std::make_pair(GLFW_KEY_RIGHT, PLAYER_MOVE_RIGHT));
	key_bindings.insert(std::make_pair(GLFW_KEY_E, PLAYER_ATTACK));
	key_bindings.insert(std::make_pair(GLFW_KEY_ESCAPE, PAUSE_OR_UNPAUSE_GAME));
}

GameOptions::~GameOptions() = default;
