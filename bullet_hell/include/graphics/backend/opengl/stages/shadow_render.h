#pragma once

#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"
#include "graphics/graph/cascade_shadow_slice.h"

class ShadowRender : RenderStage
{
public:
	ShadowRender(Shader* shader,
		StageResource<RenderBuffers>* render_buffers,
		StageResource<CascadeShadows>* cascade_shadows,
		StageResource<Framebuffer>* depth_map,
		StageResource<CommandBuffers>* command_buffers)
		: shader{ shader }
		, render_buffers{ render_buffers }
		, cascade_shadows{ cascade_shadows }
		, depth_map{ depth_map }
		, command_buffers{ command_buffers }
	{}

	void render(Scene& scene);

private:
	Shader* const shader;
	StageResource<RenderBuffers>* const render_buffers;
	StageResource<CascadeShadows>* const cascade_shadows;
	StageResource<Framebuffer>* const depth_map;
	StageResource<CommandBuffers>* const command_buffers;
};