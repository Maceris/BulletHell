#pragma once

#include "Globals.h"

#include "glad.h"
#include "glfw3.h"

#include "AnimationRender.h"
#include "CommandBuffer.h"
#include "FilterRender.h"
#include "GuiRender.h"
#include "LightRender.h"
#include "RenderBuffers.h"
#include "SceneRender.h"
#include "ShadowRender.h"
#include "SkyBoxRender.h"
#include "Window.h"

#include "GBuffer.h"

/// <summary>
/// Handles all the rendering stages for drawing to the screen
/// </summary>
class Render
{
public:
	/// <summary>
	/// Set up a rendering pipeline for the given window
	/// </summary>
	/// <param name="window"></param>
	Render(const Window& window);

	Render(const Render&) = delete;
	Render& operator=(const Render&) = delete;
	~Render();

	/// <summary>
	/// Set up uniforms for textures and materials.
	/// </summary>
	/// <param name="scene">The scene we will be rendering.</param>
	void recalculate_materials(const Scene& scene);

	/// <summary>
	/// Render the scene.
	/// </summary>
	/// <param name="window">The window we are drawing in.</param>
	/// <param name="scene">The scene to render.</param>
	void render(const Window& window, const Scene& scene);

	/// <summary>
	/// Update the buffers for when the window is resized.
	/// </summary>
	/// <param name="width">The new window width, in pixels.</param>
	/// <param name="height">The new window height, in pixels.</param>
	void resize(const unsigned int width, const unsigned int height);

	/// <summary>
	/// Set up the buffers before rendering.
	/// </summary>
	/// <param name="scene">The scene to read models from.</param>
	void setup_data(const Scene& scene);

private:

	/// <summary>
	/// The geometry buffer for deferred rendering.
	/// </summary>
	GBuffer gBuffer;

	/// <summary>
	/// The command buffers for batch rendering of models.
	/// </summary>
	CommandBuffer command_buffer;

	/// <summary>
	/// The buffers for indirect drawing of models.
	/// </summary>
	RenderBuffers render_buffers;

	/// <summary>
	/// Whether we have set up the render and command buffers for the scene.
	/// If we have, but need to set data up for the scene again, we will
	/// need to clear them out and start over.
	/// </summary>
	bool buffers_populated;

	/// <summary>
	/// The animation render stage.
	/// </summary>
	AnimationRender animation_render;

	/// <summary>
	/// The GUI render stage.
	/// </summary>
	GuiRender gui_render;

	/// <summary>
	/// The light render stage.
	/// </summary>
	LightRender light_render;

	/// <summary>
	/// The scene geometry render stage.
	/// </summary>
	SceneRender scene_render;

	/// <summary>
	/// The shadow render stage.
	/// </summary>
	ShadowRender shadow_render;

	/// <summary>
	/// The skybox render stage.
	/// </summary>
	SkyBoxRender sky_box_render;

	/// <summary>
	/// The post-processing filter stage.
	/// </summary>
	FilterRender filter_render;
	
	/// <summary>
	/// The Frame Buffer Object for the pre-filter render target.
	/// </summary>
	GLuint screen_FBO;

	/// <summary>
	/// The depth Render Buffer Object for the pre-filter render target.
	/// </summary>
	GLuint screen_RBO_depth;

	/// <summary>
	/// The texture ID we render to before applying post-processing filters.
	/// </summary>
	GLuint screen_texture;

	/// <summary>
	/// Restore the blending and frame buffer setup after we are done with 
	/// rendering lights.
	/// </summary>
	void light_render_finish();

	/// <summary>
	/// Set up the blending for rendering to the screen texture.
	/// </summary>
	/// <param name="width">The width of the window, in pixels.</param>
	/// <param name="height">The height of the window, in pixels.</param>
	void light_render_start(const unsigned int width,
		const unsigned int height);

	/// <summary>
	/// Set up the command buffer for animated models.
	/// </summary>
	/// <param name="scene">The scene we are rendering.</param>
	void setup_animated_command_buffer(const Scene& scene);

	/// <summary>
	/// Set up the command buffer for static models.
	/// </summary>
	/// <param name="scene">The scene we are rendering./</param>
	void setup_static_command_buffer(const Scene& scene);

	/// <summary>
	/// Take a list of models, and upload the model matrices to the specified 
	/// buffer.
	/// </summary>
	/// <param name="models">The list of models.</param>
	/// <param name="buffer_id">The buffer we want to send data to.</param>
	void update_model_buffer(const std::vector<std::shared_ptr<Model>> models,
		GLuint buffer_id);

	/// <summary>
	/// Update the model matrices buffers for all the scene objects.
	/// </summary>
	/// <param name="scene">The scene we are rendering.</param>
	void update_model_matrices(const Scene& scene);
};
