#pragma once

#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

class AnimationRender : RenderStage
{
public:
	AnimationRender(Shader* shader,
		StageResource<RenderBuffers>* render_buffers);

	void render(Scene& scene);

private:
	Shader* shader;
	StageResource<RenderBuffers>* const render_buffers;
};