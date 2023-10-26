#pragma once

#include "Globals.h"

#include "glm.hpp"

class MouseInput
{
public:
	/// <summary>
	/// Where within the window the mouse is. Measured in pixels with (0, 0)
	/// being the top left corner.
	/// </summary>
	glm::vec2 current_position;
	
	/// <summary>
	/// Whether the mouse is currently in the window.
	/// </summary>
	bool in_window;

	/// <summary>
	/// If the left mouse button is currently pressed.
	/// </summary>
	bool left_button_pressed;

	/// <summary>
	/// If the right mouse button is currently pressed.
	/// </summary>
	bool right_button_pressed;

	/// <summary>
	/// The displacement in pixels between the last position we measured, and
	/// the current mouse position.
	/// </summary>
	glm::vec2 displacement;

	MouseInput();
	MouseInput(const MouseInput&) = delete;
	MouseInput& operator=(const MouseInput&) = delete;
	~MouseInput() = default;

	/// <summary>
	/// Process input, mostly displacement vectors.
	/// </summary>
	void handle_input();
private:

	/// <summary>
	/// The position the mouse was in last time we checked for input. 
	/// Measured in pixels with (0, 0) being the top left corner.
	/// </summary>
	glm::vec2 previous_position;
};