#pragma once

#include "graphics/backend/base/pipeline_manager.h"
#include "graphics/frontend/pipeline.h"
#include "graphics/frontend/render_config.h"
#include "graphics/frontend/shader_map.h"

#include <string_view>

class Scene;
class Window;

class Instance
{
public:
	Instance(Window& window);
	Instance(const Instance&) = delete;
	Instance& operator=(const Instance&) = delete;
	Instance(Instance&&) = delete;
	Instance& operator=(Instance&&) = delete;
	~Instance();

	void initialize(const Window& window);
	void cleanup();
	void process_resources();
	void render(const Scene& scene);
	void resize(int width, int height);
	void setup_data(const Scene& scene);
	void set_filter(const std::string_view shader_path);
	void swap_pipeline(RenderConfig config);

private:
	//NOTE(ches) in initialization order
	ShaderMap shader_map;
	PipelineManager pipeline_manager;
	Pipeline* pipeline;
};
