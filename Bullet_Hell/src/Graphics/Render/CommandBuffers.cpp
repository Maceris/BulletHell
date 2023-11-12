#include "Graphics/Render/CommandBuffers.h"

#include "glad.h"

void CommandBuffers::cleanup()
{
	if (animated_command_buffer != 0)
	{
		glDeleteBuffers(1, &animated_command_buffer);
		animated_command_buffer = 0;
	}
	if (animated_draw_element_buffer != 0)
	{
		glDeleteBuffers(1, &animated_draw_element_buffer);
		animated_draw_element_buffer = 0;
	}
	if (animated_model_matrices_buffer != 0)
	{
		glDeleteBuffers(1, &animated_model_matrices_buffer);
		animated_model_matrices_buffer = 0;
	}
	animated_draw_count = 0;

	if (static_command_buffer != 0)
	{
		glDeleteBuffers(1, &static_command_buffer);
		static_command_buffer = 0;
	}
	if (static_draw_element_buffer != 0)
	{
		glDeleteBuffers(1, &static_draw_element_buffer);
		static_draw_element_buffer = 0;
	}
	if (static_model_matrices_buffer != 0)
	{
		glDeleteBuffers(1, &static_model_matrices_buffer);
		static_model_matrices_buffer = 0;
	}
	static_draw_count = 0;
}

CommandBuffers::CommandBuffers()
	: animated_command_buffer{ 0 }
	, animated_draw_count{ 0 }
	, animated_draw_element_buffer{ 0 }
	, animated_model_matrices_buffer{ 0 }
	, static_command_buffer{ 0 }
	, static_draw_count{ 0 }
	, static_draw_element_buffer{ 0 }
	, static_model_matrices_buffer{ 0 }
{}
	
CommandBuffers::~CommandBuffers()
{
	cleanup();
}
