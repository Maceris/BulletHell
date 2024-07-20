#pragma once

#include "graphics/backend/base/render_stage.h"
#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/shader.h"

template <bool wireframe>
class SceneRender : RenderStage
{
public:
	SceneRender(Shader* shader,
		RenderBuffers* render_buffers,
		Framebuffer* gbuffer,
		CommandBuffers* command_buffers,
		Texture* default_texture)
		: shader{ shader }
		, render_buffers{ render_buffers }
		, gbuffer{ gbuffer }
		, command_buffers{ command_buffers }
		, default_texture{ default_texture }
	{}

	void render(Scene& scene);

private:
	const Shader* shader;
	const RenderBuffers* render_buffers;
	const Framebuffer* gbuffer;
	const CommandBuffers* command_buffers;
	const Texture* default_texture;
};