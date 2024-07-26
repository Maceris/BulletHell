#pragma once

#include "graphics/glad_types.h"

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
	GLuint static_model_matrices_buffer;

	/// <summary>
	/// Delete any buffers that are set up.
	/// </summary>
	void cleanup();

	CommandBuffers();
	CommandBuffers(const CommandBuffers&) = delete;
	CommandBuffers& operator=(const CommandBuffers&) = delete;
	~CommandBuffers();
};
