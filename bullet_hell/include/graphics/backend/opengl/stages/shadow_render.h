#pragma once

#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"
#include "graphics/graph/cascade_shadow_slice.h"

class ShadowRender : public RenderStage
{
public:
	ShadowRender(StageResource<RenderBuffers>* render_buffers,
		StageResource<CascadeShadows>* cascade_shadows,
		StageResource<Framebuffer>* depth_map,
		StageResource<CommandBuffers>* command_buffers);

	void render(Scene& scene);

private:
	Shader* shader;
	StageResource<RenderBuffers>* const render_buffers;
	StageResource<CascadeShadows>* const cascade_shadows;
	StageResource<Framebuffer>* const depth_map;
	StageResource<CommandBuffers>* const command_buffers;
};