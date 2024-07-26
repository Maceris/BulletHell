#pragma once

#include "globals.h"

#include "graphics/glad_types.h"
#include "graphics/graph/gbuffer.h"
#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/render/animation_render.h"
#include "graphics/render/debug_render.h"
#include "graphics/render/filter_render.h"
#include "graphics/render/gui_render.h"
#include "graphics/render/light_render.h"
#include "graphics/render/scene_render.h"
#include "graphics/render/shadow_render.h"
#include "graphics/render/sky_box_render.h"

class Window;

/// <summary>
/// Configuration for tweaking the rendering pipeline.
/// </summary>
struct Configuration
{
	bool wireframe;
#if _DEBUG
	bool debug_lines;
#endif
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
	/// Refresh the buffers for animated models.
	/// </summary>
	/// <param name="scene">The scene to read models from.</param>
	void refresh_animated_data(Scene& scene);

	/// <summary>
	/// Refresh the buffers for static models.
	/// </summary>
	/// <param name="scene">The scene to read models from.</param>
	void refresh_static_data(Scene& scene);

	/// <summary>
	/// Render the scene.
	/// </summary>
	/// <param name="window">The window we are drawing in.</param>
	/// <param name="scene">The scene to render.</param>
	void render(const Window& window, const Scene& scene);

	/// <summary>
	/// Render the UI
	/// </summary>
	void render_just_ui(const Window& window, const Scene& scene);

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
	void setup_all_data(Scene& scene);

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

	AnimationRender animation_render;
	GuiRender gui_render;
	LightRender light_render;
	SceneRender scene_render;
	ShadowRender shadow_render;
	DebugRender debug_render;
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
	/// Set up material IDs.
	/// </summary>
	/// <param name="scene">The scene we will be rendering.</param>
	void recalculate_materials(const Scene& scene);

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
