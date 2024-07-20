#pragma once

#include "graphics/backend/base/render_stage.h"
#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/shader.h"
#include "graphics/graph/cascade_shadow_slice.h"

class ShadowRender : RenderStage
{
public:
	ShadowRender(Shader* shader,
		RenderBuffers* render_buffers,
		CascadeShadows* cascade_shadows,
		Framebuffer* depth_map,
		CommandBuffers* command_buffers)
		: shader{ shader }
		, render_buffers{ render_buffers }
		, cascade_shadows{ cascade_shadows }
		, depth_map{ depth_map }
		, command_buffers{ command_buffers }
	{}

	void render(Scene& scene);

private:
	const Shader* shader;
	const RenderBuffers* render_buffers;
	const CascadeShadows* cascade_shadows;
	const Framebuffer* depth_map;
	const CommandBuffers* command_buffers;
};