#pragma once

#include "Globals.h"

#include "glad.h"
#define GLFW_INCLUDE_NONE
#include "glfw3.h"

#include <vector>

#include "Scene.h"

class RenderBuffers
{
public:
	// Prepares to be set up, but models must be loaded before use
	RenderBuffers();
	
	// Performs cleanup
	~RenderBuffers();

	// Clean up all the buffers and data
	void cleanup();

	/*
	Load models with animation data in a scene. Make sure to clear out the
	existing buffer data before doing this a second time.
	*/
	void load_animated_models(const Scene& scene);

	/*
	Load models with no animation data in a scene. Make sure to clear out the
	existing buffer data before doing this a second time.
	*/
	void load_static_models(const Scene& scene);

private:
	// The Vertex Array Object ID for animated models
	GLuint animated_VAO;
	
	// The Vertex Buffer Object ID for binding poses of animated models
	GLuint binding_poses_VBO;

	/*
	The Vertex Buffer Object ID for indices and weights of the bones of
	animated models
	*/
	GLuint bones_indices_weights_VBO;

	// The Vertex Buffer Object ID for bones matrices of animated models
	GLuint bones_matrices_VBO;

	/*
	The Vertex Buffer Object ID for the transformed animation vertices
	after processing of animated models
	*/
	GLuint dest_animation_VBO;

	// The Vertex Array Object ID for static models
	GLuint static_VAO;

	// A list of all the VBOs that have been set up
	std::vector<GLuint> VBO_list;
};