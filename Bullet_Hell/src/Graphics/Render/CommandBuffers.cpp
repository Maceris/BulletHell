#include "Graphics/Render/CommandBuffers.h"

#include "glad.h"

void CommandBuffers::cleanup()
{
	if (animated_command_buffer != 0)
	{
		glDeleteBuffers(1, &animated_command_buffer);
	}
	if (animated_draw_element_buffer != 0)
	{
		glDeleteBuffers(1, &animated_draw_element_buffer);
	}
	if (animated_model_matrices_buffer != 0)
	{
		glDeleteBuffers(1, &animated_model_matrices_buffer);
	}

	if (static_command_buffer != 0)
	{
		glDeleteBuffers(1, &static_command_buffer);
	}
	if (static_draw_element_buffer != 0)
	{
		glDeleteBuffers(1, &static_draw_element_buffer);
	}
	if (static_model_matrices_buffer != 0)
	{
		glDeleteBuffers(1, &static_model_matrices_buffer);
	}
}

CommandBuffers::CommandBuffers()
	: animated_command_buffer(0)
	, animated_draw_count(0)
	, animated_draw_element_buffer(0)
	, animated_model_matrices_buffer(0)
	, static_command_buffer(0)
	, static_draw_count(0)
	, static_draw_element_buffer(0)
	, static_model_matrices_buffer(0)
{}
	
CommandBuffers::~CommandBuffers()
{
	cleanup();
}
