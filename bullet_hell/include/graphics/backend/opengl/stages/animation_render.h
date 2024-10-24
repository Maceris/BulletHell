#pragma once

#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

class AnimationRender : public RenderStage
{
public:
	AnimationRender(StageResource<RenderBuffers>* render_buffers);
	virtual ~AnimationRender() = default;

	void render(Scene& scene);

private:
	Shader* shader;
	StageResource<RenderBuffers>* const render_buffers;
};