#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <map>

#include "glm/gtc/type_ptr.hpp"

#include "debugging/logger.h"
#include "debugging/timer.h"
#include "graphics/render_constants.h"
#include "graphics/scene/scene.h"
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
#include "graphics/frontend/instance.h"
#include "memory/memory_util.h"

Texture* PipelineManager::default_texture = nullptr;

struct PipelineManager::Data
{
	Data(Window& window);
	Data(const Data&) = delete;
	Data& operator=(const Data&) = delete;
	~Data();


	std::map<RenderConfig, Pipeline*> pipelines;
};

PipelineManager::Data::Data(Window& window)
{}

PipelineManager::Data::~Data()
{

}

PipelineManager::PipelineManager(Window& window, 
	DeletionQueue* const deletion_queue, ShaderMap& shaders)
	: data{ std::make_unique<Data>(window) }
	, deletion_queue{ deletion_queue }
{
	
}

PipelineManager::~PipelineManager()
{
	
}

Pipeline* PipelineManager::get_pipeline(RenderConfig config)
{
	if (data->pipelines.contains(config))
	{
		return data->pipelines.find(config)->second;
	}

	Pipeline* result = build_pipeline(config);

	data->pipelines.insert(std::pair(config, result));

	return result;
}

void PipelineManager::resize(int width, int height)
{
}

void PipelineManager::setup_data(Scene& scene)
{
	
}

Pipeline* PipelineManager::build_pipeline(RenderConfig config)
{
	Pipeline* result = ALLOC Pipeline();

	return result;
}

#endif
