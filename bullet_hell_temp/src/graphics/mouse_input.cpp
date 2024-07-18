#include "Graphics/MouseInput.h"

MouseInput::MouseInput()
	: current_position{ 0 }
	, in_window{ false }
	, left_button_pressed{ false }
	, right_button_pressed{ false }
	, displacement{ 0 }
	, previous_position{ 0 }
{}

void MouseInput::handle_input()
{
	displacement.x = 0;
	displacement.y = 0;
	if (previous_position.x > 0 && previous_position.y > 0 && in_window)
	{
		glm::vec2 delta = current_position - previous_position;
		//NOTE(ches) not a typo, we need to swap coordinates
		if (delta.x != 0)
		{
			displacement.y = delta.x;
		}
		if (delta.y != 0)
		{
			displacement.x = delta.y;
		}
	}
	previous_position = current_position;
}