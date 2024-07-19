#include "main/game_options.h"

#include "GLFW/glfw3.h"

GameOptions::GameOptions()
	: window_title{ "Bullet Hell" }
	, key_bindings{}
{
#if DEBUG
	key_bindings.insert(std::make_pair(GLFW_KEY_UP, Action::CAMERA_MOVE_FORWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_DOWN, Action::CAMERA_MOVE_BACKWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_LEFT, Action::CAMERA_MOVE_LEFT));
	key_bindings.insert(std::make_pair(GLFW_KEY_RIGHT, Action::CAMERA_MOVE_RIGHT));
	key_bindings.insert(std::make_pair(GLFW_KEY_LEFT_SHIFT, Action::CAMERA_MOVE_DOWN));
	key_bindings.insert(std::make_pair(GLFW_KEY_Z, Action::CAMERA_MOVE_UP));
#endif

	key_bindings.insert(std::make_pair(GLFW_KEY_W, Action::PLAYER_MOVE_FORWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_S, Action::PLAYER_MOVE_BACKWARD));
	key_bindings.insert(std::make_pair(GLFW_KEY_A, Action::PLAYER_MOVE_LEFT));
	key_bindings.insert(std::make_pair(GLFW_KEY_D, Action::PLAYER_MOVE_RIGHT));
	key_bindings.insert(std::make_pair(GLFW_KEY_SPACE, Action::PLAYER_ATTACK));

	key_bindings.insert(std::make_pair(GLFW_KEY_ESCAPE, Action::PAUSE_OR_UNPAUSE_GAME));
}

GameOptions::~GameOptions() = default;
