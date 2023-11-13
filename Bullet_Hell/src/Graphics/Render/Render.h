#pragma once

#include "Globals.h"

#include "Graphics/GladTypes.h"
#include "Graphics/Window.h"
#include "Graphics/Graph/GBuffer.h"
#include "Graphics/Render/AnimationRender.h"
#include "Graphics/Render/CommandBuffers.h"
#include "Graphics/Render/FilterRender.h"
#include "Graphics/Render/GuiRender.h"
#include "Graphics/Render/LightRender.h"
#include "Graphics/Render/RenderBuffers.h"
#include "Graphics/Render/SceneRender.h"
#include "Graphics/Render/ShadowRender.h"
#include "Graphics/Render/SkyBoxRender.h"

/// <summary>
/// Configuration for tweaking the rendering pipeline.
/// </summary>
struct Configuration
{
	bool wireframe;
};

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

	/// <summary>
	/// Configuration for tweaking the rendering pipeline.
	/// </summary>
	static Configuration configuration;
private:

	/// <summary>
	/// The geometry buffer for deferred rendering.
	/// </summary>
	GBuffer gBuffer;

	/// <summary>
	/// The command buffers for batch rendering of models.
	/// </summary>
	CommandBuffers command_buffers;

	/// <summary>
	/// The buffers for indirect drawing of models.
	/// </summary>
	RenderBuffers render_buffers;

	/// <summary>
	/// Whether we have set up the render and command buffers for the scene.
	/// If we have, but need to set data up for the scene again, we will
	/// need to clear them out and start over.
	/// </summary>
	std::atomic_bool buffers_populated;

	AnimationRender animation_render;
	GuiRender gui_render;
	LightRender light_render;
	SceneRender scene_render;
	ShadowRender shadow_render;
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
	/// Set up the command buffers to render animated models, which should be
	/// deleted before calling this if they are currently filled.
	/// </summary>
	/// <param name="scene">The scene we are rendering.</param>
	void setup_animated_command_buffer(const Scene& scene);

	/// <summary>
	/// Set up the command buffers to render static models, which should be
	/// deleted before calling this if they are currently filled.
	/// </summary>
	/// <param name="scene">The scene we are rendering.</param>
	void setup_static_command_buffer(const Scene& scene);

	/// <summary>
	/// Take a list of models, and upload the model matrices to the specified 
	/// buffer.
	/// </summary>
	/// <param name="models">The list of models.</param>
	/// <param name="buffer_id">The buffer we want to send data to.</param>
	void update_model_buffer(const std::vector<std::shared_ptr<Model>> models,
		GLuint buffer_id);

	void update_model_matrices(const Scene& scene);
};
