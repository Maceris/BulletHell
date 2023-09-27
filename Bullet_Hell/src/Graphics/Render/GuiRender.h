#pragma once

#include "Globals.h"

#include "glm.hpp"

#include "Scene.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "UniformsMap.h"
#include "Window.h"

#include <memory>

/// <summary>
/// Handles Graphical User Interface rendering
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
	glm::vec2 scale;
	std::unique_ptr<ShaderProgram> shader_program;
	Texture font;
	UniformsMap uniforms_map;

	void create_ui_resources(const Window& window);
	void create_uniforms();
	void setup_imgui_keys();
};
