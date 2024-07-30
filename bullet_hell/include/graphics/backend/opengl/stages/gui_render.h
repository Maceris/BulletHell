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
	GuiRender(const Window& window, StageResource<GuiMesh>* gui_mesh);
	GuiRender(const GuiRender&) = delete;
	GuiRender& operator=(const GuiRender&) = delete;
	~GuiRender() = default;

	void render(const Scene& scene);

protected:

	Shader* shader;
	StageResource<GuiMesh>* gui_mesh;
	glm::vec2 scale;

	void inline render_gui(const Scene& scene);
};

class GuiRenderStandalone : GuiRender
{
	
};