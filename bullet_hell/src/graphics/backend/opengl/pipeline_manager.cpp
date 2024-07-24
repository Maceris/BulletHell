#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL || BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/backend/base/pipeline_manager.h"

#include "graphics/backend/opengl/stages/animation_render.h"
#include "graphics/backend/opengl/stages/debug_render.h"
#include "graphics/backend/opengl/stages/filter_render.h"
#include "graphics/backend/opengl/stages/framebuffer_transition.h"
#include "graphics/backend/opengl/stages/gui_render.h"
#include "graphics/backend/opengl/stages/light_render.h"
#include "graphics/backend/opengl/stages/model_matrix_update.h"
#include "graphics/backend/opengl/stages/scene_render.h"
#include "graphics/backend/opengl/stages/shadow_render.h"
#include "graphics/backend/opengl/stages/skybox_render.h"

std::unique_ptr<Texture> PipelineManager::default_texture = nullptr;

struct PipelineManager::Data
{
	AnimationRender animation_render;
	FramebufferTransition back_buffer_binding;
	FramebufferTransition screen_texture_binding;
	FilterRender filter_render;
	GuiRender<false> gui_render;
	GuiRender<true> gui_render_standalone;
	LightRender light_render;
	ModelMatrixUpdate model_matrix_update;
	SceneRender<false> scene_render;
	SceneRender<true> scene_render_wireframe;
	ShadowRender shadow_render;
	SkyboxRender skybox_render;
};

PipelineManager::PipelineManager(Window& window, ShaderMap& shaders)
{

}

PipelineManager::~PipelineManager()
{

}

void PipelineManager::cleanup()
{

}

Pipeline* PipelineManager::get_pipeline(RenderConfig config)
{
	return nullptr;
}

void PipelineManager::resize(int width, int height)
{

}

void PipelineManager::setup_data(const Scene& scene)
{

}

Pipeline* PipelineManager::build_pipeline(RenderConfig config)
{
	return nullptr;
}

#endif
