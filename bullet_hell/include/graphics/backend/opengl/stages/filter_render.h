#pragma once

#include "graphics/backend/base/render_stage.h"
#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/shader.h"

class FilterRender : RenderStage
{
public:
	FilterRender(Shader* shader,
		Framebuffer* scene_texture,
		QuadMesh* quad_mesh)
		: shader{ shader }
		, scene_texture{ scene_texture }
		, quad_mesh{ quad_mesh }
	{}

	void render(Scene& scene);

private:
	Shader* const shader;
	Framebuffer* const scene_texture;
	QuadMesh* const quad_mesh;
};