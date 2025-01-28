#pragma once

#include "graphics/backend/base/pipeline_manager.h"
#include "graphics/frontend/deletion_queue.h"
#include "graphics/frontend/pipeline.h"
#include "graphics/frontend/render_config.h"
#include "graphics/frontend/shader_map.h"

#include <string_view>

class Scene;
class Window;

/// <summary>
/// Configuration for tweaking the rendering pipeline.
/// </summary>
struct Configuration
{
	bool wireframe;
#if _DEBUG
	bool debug_lines;
#endif
};

class Instance
{
	friend class PipelineManager;

public:
	Instance();
	Instance(const Instance&) = delete;
	Instance& operator=(const Instance&) = delete;
	Instance(Instance&&) = delete;
	Instance& operator=(Instance&&) = delete;
	~Instance();

	/// <summary>
	/// Create a swap chain. Should only be called directly near
	/// initialization, once one is created recreate_swap_chain should be used.
	/// </summary>
	/// <param name="window"></param>
	void create_swap_chain();

	/// <summary>
	/// Early initialization of the instance.
	/// </summary>
	void initialize();
	void initialize_pipeline_manager();
	void process_resources();
	void recreate_swap_chain();
	void render(Scene& scene);
	void resize(int width, int height);
	void setup_data(Scene& scene);
	void set_filter(const std::string_view shader_path);
	void swap_pipeline(RenderConfig config);

	/// <summary>
	/// Configuration for tweaking the rendering pipeline.
	/// </summary>
	static Configuration configuration;

private:
	//NOTE(ches) in initialization order
	DeletionQueue deletion_queue;
	ShaderMap shader_map;
	PipelineManager* pipeline_manager;
	Pipeline* pipeline;

	struct Data;
	const std::unique_ptr<Data> data;
};
