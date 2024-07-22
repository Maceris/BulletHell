#pragma once

#include "glm/vec2.hpp"

#include "graphics/backend/base/render_stage.h"
#include "graphics/backend/opengl/gui_mesh.h"
#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/shader.h"

template <bool standalone>
class GuiRender : RenderStage
{
public:
	GuiRender(Shader* shader,
		GuiMesh* gui_mesh,
		const glm::vec2 scale)
		: shader{ shader }
		, scene_texture{ scene_texture }
		, scale{ std::move(scale) }
	{}

	void render(Scene& scene);

private:
	Shader* const shader;
	Framebuffer* const scene_texture;
	glm::vec2 scale;
};