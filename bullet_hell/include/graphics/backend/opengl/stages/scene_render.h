#pragma once

#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

class SceneRender : public RenderStage
{
public:
	SceneRender(StageResource<RenderBuffers>* render_buffers,
		StageResource<Framebuffer>* gbuffer,
		StageResource<CommandBuffers>* command_buffers,
		StageResource<Texture>* default_texture);
	SceneRender(const SceneRender&) = delete;
	SceneRender& operator=(const SceneRender&) = delete;
	virtual ~SceneRender() = default;

	virtual void render(Scene& scene);
	void create_uniforms();

protected:
	Shader* shader;
	StageResource<RenderBuffers>* const render_buffers;
	StageResource<Framebuffer>* const gbuffer;
	StageResource<CommandBuffers>* const command_buffers;
	StageResource<Texture>* const default_texture;

	/// <summary>
	/// Set up the uniforms for models in the scene.
	/// </summary>
	/// <param name="scene">The model we are going to render.</param>
	void setup_materials_uniform(Scene& scene, const bool animated);

	void inline common_scene_render(Scene& scene);
};

class SceneRenderWireframe : public SceneRender
{
public:
	SceneRenderWireframe(StageResource<RenderBuffers>* render_buffers,
		StageResource<Framebuffer>* gbuffer,
		StageResource<CommandBuffers>* command_buffers,
		StageResource<Texture>* default_texture)
		: SceneRender{ render_buffers, gbuffer, command_buffers, 
			default_texture }
	{}

	virtual void render(Scene& scene);

	SceneRenderWireframe(const SceneRenderWireframe&) = delete;
	SceneRenderWireframe& operator=(const SceneRenderWireframe&) = delete;
	virtual ~SceneRenderWireframe() = default;
};
