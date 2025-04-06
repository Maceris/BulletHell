#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include <vector>

#include "debugging/timer.h"
#include "graphics/frontend/pipeline.h"
#include "graphics/frontend/render_stage.h"

//TODO(ches) fill this out

Pipeline::Pipeline() = default;

Pipeline::~Pipeline() = default;

void Pipeline::render(Scene& scene, ShaderMap& shaders)
{
	TIME_END("Last Frame");
	TIME_START("Last Frame");
	for (auto& stage : render_stages)
	{
		stage->render(scene);
	}
}

#endif