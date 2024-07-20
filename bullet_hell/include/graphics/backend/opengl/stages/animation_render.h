#pragma once

#include "graphics/backend/base/render_stage.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/shader.h"

class AnimationRender : RenderStage
{
public:
	AnimationRender(Shader* shader,
		const RenderBuffers* render_buffers)
		: shader{ shader }
		, render_buffers{ render_buffers }
	{}

	void render(Scene& scene);

private:
	const Shader* shader;
	const RenderBuffers* render_buffers;
};