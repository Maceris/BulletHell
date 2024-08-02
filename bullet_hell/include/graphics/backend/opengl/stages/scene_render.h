#pragma once

#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

class SceneRender : RenderStage
{
public:
	SceneRender(StageResource<RenderBuffers>* render_buffers,
		StageResource<Framebuffer>* gbuffer,
		StageResource<CommandBuffers>* command_buffers,
		StageResource<Texture>* default_texture);
	SceneRender(const SceneRender&) = delete;
	SceneRender& operator=(const SceneRender&) = delete;
	~SceneRender() = default;

	void render(Scene& scene);
	void create_uniforms();

private:
	Shader* shader;
	StageResource<RenderBuffers>* const render_buffers;
	StageResource<Framebuffer>* const gbuffer;
	StageResource<CommandBuffers>* const command_buffers;
	StageResource<Texture>* const default_texture;
};

class SceneRenderWireframe : SceneRender
{
public:
	SceneRenderWireframe(StageResource<RenderBuffers>* render_buffers,
		StageResource<Framebuffer>* gbuffer,
		StageResource<CommandBuffers>* command_buffers,
		StageResource<Texture>* default_texture)
		: SceneRender{ render_buffers, gbuffer, command_buffers, 
			default_texture }
	{}

	SceneRenderWireframe(const SceneRenderWireframe&) = delete;
	SceneRenderWireframe& operator=(const SceneRenderWireframe&) = delete;
	~SceneRenderWireframe() = default;
};
