#pragma once

#include <memory>
#include <vector>

#include "graphics/frontend/render_stage.h"

class Window;
struct ShaderMap;

/// <summary>
/// Represents the rendering pipeline. Not to be confused with things like a
/// Vulkan pipeline, as this handles all rendering of a frame from start to
/// finish, involving potentially multiple stages, shaders, and render targets.
/// </summary>
class Pipeline
{
public:
	Pipeline();
	Pipeline(const Pipeline&) = delete;
	Pipeline& operator=(const Pipeline&) = delete;
	~Pipeline();

	void initialize(const Window& window, ShaderMap& shaders);
	void render(const Window& window, ShaderMap& shaders);

	std::vector<RenderStage*> render_stages;
};
