#pragma once

#include "graphics/frontend/render_config.h"

class Scene;
class Window;

class Instance
{
public:
	Instance();
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
	void swap_pipeline(RenderConfig config);
};
