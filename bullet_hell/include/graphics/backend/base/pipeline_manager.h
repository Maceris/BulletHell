#pragma once

#include <memory>

#include "graphics/window.h"
#include "graphics/frontend/pipeline.h"
#include "graphics/frontend/render_config.h"
#include "graphics/frontend/shader_map.h"
#include "graphics/frontend/texture.h"

class PipelineManager
{
public:
	struct Data;
	static Texture* default_texture;

	PipelineManager(Window& window, ShaderMap& shaders);
	PipelineManager(const PipelineManager&) = delete;
	PipelineManager& operator=(const PipelineManager&) = delete;
	~PipelineManager();

	Pipeline* get_pipeline(RenderConfig config);
	void resize(int width, int height);
	void setup_data(const Scene& scene);

private:
	const std::unique_ptr<Data> data;

	Pipeline* build_pipeline(RenderConfig config);
};