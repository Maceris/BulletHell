#include "main/game_options.h"

#include "GLFW/glfw3.h"

GameOptions::GameOptions()
	: window_title{ "Bullet Hell" }
	, key_bindings{}
{
#if DEBUG
	key_bindings.insert(std::make_pair(GLFW_KEY_UP, CAMERA_MOVE_FORWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_DOWN, CAMERA_MOVE_BACKWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_LEFT, CAMERA_MOVE_LEFT));
	key_bindings.insert(std::make_pair(GLFW_KEY_RIGHT, CAMERA_MOVE_RIGHT));
	key_bindings.insert(std::make_pair(GLFW_KEY_LEFT_SHIFT, CAMERA_MOVE_DOWN));
	key_bindings.insert(std::make_pair(GLFW_KEY_Z, CAMERA_MOVE_UP));
#endif

	key_bindings.insert(std::make_pair(GLFW_KEY_W, PLAYER_MOVE_FORWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_S, PLAYER_MOVE_BACKWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_A, PLAYER_MOVE_LEFT));
	key_bindings.insert(std::make_pair(GLFW_KEY_D, PLAYER_MOVE_RIGHT));
	key_bindings.insert(std::make_pair(GLFW_KEY_SPACE, PLAYER_ATTACK));

	key_bindings.insert(std::make_pair(GLFW_KEY_ESCAPE, PAUSE_OR_UNPAUSE_GAME));
}

GameOptions::~GameOptions() = default;
