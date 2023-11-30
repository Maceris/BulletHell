#pragma once

#include "Globals.h"

#include <vector>

#include "Graphics/GladTypes.h"
#include "Graphics/Graph/Model.h"

class Scene;

/// <summary>
/// A list of pointers to models.
/// </summary>
using ModelList = std::vector<std::shared_ptr<Model>>;

/// <summary>
/// Buffers for indirect drawing of models.
/// </summary>
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
	/// to reload the scene data into the buffers. Will not do anything if
	/// we know that the buffers are empty.
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

	/// <summary>
	/// The Vertex Array Object ID for animated models.
	/// </summary>
	GLuint animated_vao = 0;

	/// <summary>
	/// The Vertex Array Object ID for static models.
	/// </summary>
	GLuint static_vao = 0;

	/// <summary>
	/// A list of all the VBOs that have been set up.
	/// </summary>
	std::vector<GLuint> vbo_list;
	
	/// <summary>
	/// The storage for binding poses of animated models.
	/// </summary>
	GLuint binding_poses_ssbo = 0;

	/// <summary>
	/// The storage for indices and weights of the bones of
	/// animated models.
	/// </summary>
	GLuint bones_indices_weights_ssbo = 0;

	/// <summary>
	/// The storage for bones matrices of animated models.
	/// </summary>
	GLuint bones_matrices_ssbo = 0;

	/// <summary>
	/// The storage for the transformed animation vertices
	/// after processing of animated models.
	/// </summary>
	GLuint dest_animation_vbo = 0;

	/// <summary>
	/// The storage for the draw parameters of the animation compute shader.
	/// </summary>
	GLuint animation_draw_parameters_ssbo = 0;

private:

	GLuint animated_index_vbo = 0;
	GLuint static_meshes_vbo = 0;
	GLuint static_index_vbo = 0;

	/// <summary>
	/// If we have populated any buffer since the last time we cleared them
	/// out.
	/// </summary>
	bool buffers_populated;
	
	/// <summary>
	/// Used to define the vertex attribute arrays.
	/// </summary>
	void define_vertex_attributes();

	/// <summary>
	/// Store binding pose information for all of the meshes that are
	/// associated with the animated models.
	/// </summary>
	/// <param name="models">The models to load binding pose information for.
	/// </param>
	void load_binding_poses(const ModelList& models);

	/// <summary>
	/// Store all of the bone index and weight information for the animated 
	/// models.
	/// </summary>
	/// <param name="models">The list of models to load bone index and weight
	/// information for.</param>
	void load_bones_indices_weights(const ModelList& models);

	/// <summary>
	/// Store the bone matrices for all animations of the animated models.
	/// </summary>
	/// <param name="models">The list of models to store bone matrices
	/// information for.</param>
	void load_bones_matrices_buffer(const ModelList& models);
};