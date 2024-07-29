#pragma once

#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

template <bool wireframe>
class SceneRender : RenderStage
{
public:
	SceneRender(StageResource<RenderBuffers>* render_buffers,
		StageResource<Framebuffer>* gbuffer,
		StageResource<CommandBuffers>* command_buffers,
		StageResource<Texture>* default_texture)
		: render_buffers{ render_buffers }
		, gbuffer{ gbuffer }
		, command_buffers{ command_buffers }
		, default_texture{ default_texture }
	{
		//TODO(ches) set up shader
	}

	void render(Scene& scene);

private:
	Shader* shader;
	StageResource<RenderBuffers>* const render_buffers;
	StageResource<Framebuffer>* const gbuffer;
	StageResource<CommandBuffers>* const command_buffers;
	StageResource<Texture>* const default_texture;
};