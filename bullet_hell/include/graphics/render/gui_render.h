#pragma once

#include <memory>

#include "glm/vec2.hpp"

#include "graphics/frontend/uniforms_map.h"
#include "graphics/graph/shader_program.h"

class Scene;
class Window;

/// <summary>
/// Handles Graphical User Interface rendering.
/// </summary>
class GuiRender
{
public:
	GuiRender(const Window& window);
	GuiRender(const GuiRender&) = delete;
	GuiRender& operator=(const GuiRender&) = delete;
	~GuiRender();

	void render(const Scene& scene);
	void resize(const unsigned int width, const unsigned int height);
private:
	/// <summary>
	/// The scale of the screen, which we need for uniforms.
	/// </summary>
	glm::vec2 scale;

	/// <summary>
	/// The GUI Shader program.
	/// </summary>
	std::unique_ptr<ShaderProgram> shader_program;

	/// <summary>
	/// Uniforms for the shader.
	/// </summary>
	std::unique_ptr<UniformsMap> uniforms_map;

	/// <summary>
	/// Set up ImGui and create fonts, textures, meshes, etc.
	/// </summary>
	/// <param name="window">The window we are setting up for.</param>
	void create_ui_resources(const Window& window);

	/// <summary>
	/// Set up the uniform map.
	/// </summary>
	void create_uniforms();
};
