#pragma once

#include <memory>

#include "graphics/frontend/render_config.h"

class Scene;
class TextureLoader;
class Window;

class Instance
{
	void initialize(const Window& window);
	void cleanup();
	void process_resources();
	void render(const Scene& scene);
	void resize(uint32_t width, uint32_t height);
	void setup_data(const Scene& scene);
	void swap_pipeline(RenderConfig config);
};
