#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <vector>

#include "debugging/timer.h"
#include "graphics/frontend/pipeline.h"
#include "graphics/frontend/render_stage.h"

Pipeline::Pipeline() = default;

Pipeline::~Pipeline() = default;

void Pipeline::render(Scene& scene, ShaderMap& shaders)
{

}

#endif