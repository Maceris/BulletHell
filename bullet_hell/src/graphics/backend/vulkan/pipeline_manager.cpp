#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <map>

#include "glm/gtc/type_ptr.hpp"

#include "debugging/logger.h"
#include "debugging/timer.h"
#include "graphics/render_constants.h"
#include "graphics/scene/scene.h"
#include "graphics/backend/base/pipeline_manager.h"
#include "graphics/backend/vulkan/global_state.h"
#include "graphics/frontend/instance.h"
#include "memory/memory_util.h"

Texture* PipelineManager::default_texture = nullptr;

struct PipelineManager::Data
{
	Data(const Window& window);
	Data(const Data&) = delete;
	Data& operator=(const Data&) = delete;
	~Data();

	std::map<RenderConfig, Pipeline*> pipelines;
};

PipelineManager::Data::Data(const Window& window)
{}

PipelineManager::Data::~Data()
{

}

PipelineManager::PipelineManager(const Window& window, 
	DeletionQueue* const deletion_queue, ShaderMap& shaders)
	: data{ std::make_unique<Data>(window) }
	, deletion_queue{ deletion_queue }
{
	
}

PipelineManager::~PipelineManager()
{
	
}

void PipelineManager::create_frame_buffers()
{
	//TODO(ches) implement this
}

void PipelineManager::destroy_frame_buffers()
{
	//TODO(ches) implement this
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
