#pragma once

#include "graphics/backend/base/render_stage.h"
#include "graphics/frontend/shader.h"

class DebugRender : RenderStage
{
public:
	DebugRender(Shader* shader)
		: shader{ shader }
	{}

	void render(Scene& scene);

private:
	Shader* const shader;
};
