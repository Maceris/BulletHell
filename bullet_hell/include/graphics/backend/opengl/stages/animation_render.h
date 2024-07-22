#pragma once

#include "graphics/backend/base/render_stage.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/shader.h"

class AnimationRender : RenderStage
{
public:
	AnimationRender(Shader* shader,
		RenderBuffers* render_buffers)
		: shader{ shader }
		, render_buffers{ render_buffers }
	{}

	void render(Scene& scene);

private:
	Shader* const shader;
	RenderBuffers* const render_buffers;
};