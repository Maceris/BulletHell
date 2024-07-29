#pragma once

#include "glm/vec2.hpp"

#include "graphics/backend/opengl/gui_mesh.h"
#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

class Window;

class GuiRender : RenderStage
{
public:
	GuiRender(StageResource<GuiMesh>* gui_mesh);

	void render(Scene& scene);

protected:

	Shader* shader;
	StageResource<GuiMesh>* gui_mesh;
	glm::vec2 scale;


	/// <summary>
	/// Set up ImGui and create fonts, textures, meshes, etc.
	/// </summary>
	/// <param name="window">The window we are setting up for.</param>
	void create_ui_resources(const Window& window);

	/// <summary>
	/// Set up the uniform map.
	/// </summary>
	void create_uniforms();
	
	void inline render_gui(Scene& scene);
};

class GuiRenderStandalone : GuiRender
{
	
};