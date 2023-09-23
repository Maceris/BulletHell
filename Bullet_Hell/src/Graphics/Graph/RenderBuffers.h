#pragma once

#include "Globals.h"

#include "glad.h"
#include "glfw3.h"

#include <vector>

#include "MeshDrawData.h"
#include "Scene.h"

class RenderBuffers
{
public:
	
	/// <summary>
	/// Prepares to be set up, but models must be loaded before use.
	/// </summary>
	RenderBuffers();
	
	/// <summary>
	/// Clean up data.
	/// </summary>
	~RenderBuffers();

	/// <summary>
	/// Clean up all the buffers and data. Should be called every time we need
	/// to reload the scene data into the buffers.
	/// </summary>
	void cleanup();

	/// <summary>
	/// Load models with animation data in a scene. Make sure to clear out the
	/// existing buffer data before doing this a second time.
	/// </summary>
	/// <param name="scene">The scene to load data from.</param>
	void load_animated_models(const Scene& scene);

	/// <summary>
	/// Load models with no animation data in a scene. Make sure to clear out
	/// the existing buffer data before doing this a second time.
	/// </summary>
	/// <param name="scene">The scene to load data from.</param>
	void load_static_models(const Scene& scene);

private:
	/// <summary>
	/// The Vertex Array Object ID for animated models.
	/// </summary>
	GLuint animated_VAO;
	
	/// <summary>
	/// The Vertex Buffer Object ID for binding poses of animated models.
	/// </summary>
	GLuint binding_poses_VBO;

	/// <summary>
	/// The Vertex Buffer Object ID for indices and weights of the bones of
	/// animated models.
	/// </summary>
	GLuint bones_indices_weights_VBO;

	/// <summary>
	/// The Vertex Buffer Object ID for bones matrices of animated models.
	/// </summary>
	GLuint bones_matrices_VBO;

	/// <summary>
	/// The Vertex Buffer Object ID for the transformed animation vertices
	/// after processing of animated models.
	/// </summary>
	GLuint dest_animation_VBO;

	/// <summary>
	/// The Vertex Array Object ID for static models.
	/// </summary>
	GLuint static_VAO;

	/// <summary>
	/// A list of all the VBOs that have been set up.
	/// </summary>
	std::vector<GLuint> VBO_list;
};