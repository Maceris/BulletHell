#pragma once

#include "Globals.h"

#include "glad.h"

/// <summary>
/// References to buffers for rendering entities, so buffers can be cached.
/// </summary>
struct CommandBuffers
{
	/// <summary>
	/// Command buffers for rendering only the animated models.
	/// </summary>
	GLuint animated_command_buffer;

	/// <summary>
	/// How many animated model draw commands are set up.
	/// </summary>
	unsigned int animated_draw_count;

	/// <summary>
	/// Storage for draw elements for animated models.
	/// </summary>
	GLuint animated_draw_element_buffer;

	/// <summary>
	/// Storage for model matrices for animated models.
	/// </summary>
	GLuint animated_model_matrices_buffer;

	/// <summary>
	/// Command buffers for rendering only the static (non-animated) models.
	/// </summary>
	GLuint static_command_buffer;

	/// <summary>
	/// How many static model draw commands are set up.
	/// </summary>
	unsigned int static_draw_count;

	/// <summary>
	/// Storage for draw elements for static models.
	/// </summary>
	GLuint static_draw_element_buffer;

	/// <summary>
	/// Storage for model matrices for static models.
	/// </summary>
	GLuint static_model_matrics_buffer;

	CommandBuffers()
		: animated_command_buffer(0)
		, animated_draw_count(0)
		, animated_draw_element_buffer(0)
		, animated_model_matrices_buffer(0)
		, static_command_buffer(0)
		, static_draw_count(0)
		, static_draw_element_buffer(0)
		, static_model_matrics_buffer(0)
	{}
	CommandBuffers(const CommandBuffers&) = delete;
	CommandBuffers& operator=(const CommandBuffers&) = delete;
	~CommandBuffers()
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
		if (static_model_matrics_buffer != 0)
		{
			glDeleteBuffers(1, &static_model_matrics_buffer);
		}
	}
};
