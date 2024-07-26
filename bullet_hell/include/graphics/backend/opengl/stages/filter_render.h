#pragma once

#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

class FilterRender : RenderStage
{
public:
	FilterRender(Shader* shader,
		StageResource<Framebuffer>* scene_texture,
		StageResource<QuadMesh>* quad_mesh)
		: shader{ shader }
		, scene_texture{ scene_texture }
		, quad_mesh{ quad_mesh }
	{}

	void render(Scene& scene);

private:
	Shader* const shader;
	StageResource<Framebuffer>* const scene_texture;
	StageResource<QuadMesh>* const quad_mesh;
};