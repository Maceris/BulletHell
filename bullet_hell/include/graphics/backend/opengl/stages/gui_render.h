#pragma once

#include "glm/vec2.hpp"

#include "graphics/backend/opengl/gui_mesh.h"
#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

template <bool standalone>
class GuiRender : RenderStage
{
public:
	GuiRender(Shader* shader,
		StageResource<GuiMesh>* gui_mesh)
		: shader{ shader }
		, gui_mesh{ gui_mesh }
		, scale{ 0, 0 }
	{}

	void render(Scene& scene);

private:
	Shader* const shader;
	StageResource<GuiMesh>* gui_mesh;
	glm::vec2 scale;
};