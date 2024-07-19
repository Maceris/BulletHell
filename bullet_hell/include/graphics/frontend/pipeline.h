#pragma once

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
	void initialize(const Window& window, ShaderMap& shaders);
	void render(const Window& window, ShaderMap& shaders);
};
