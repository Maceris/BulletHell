#pragma once

#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

class FilterRender : public RenderStage
{
public:
	FilterRender(StageResource<Framebuffer>* scene_texture,
		StageResource<QuadMesh>* quad_mesh)
		: scene_texture{ scene_texture }
		, quad_mesh{ quad_mesh }
	{
		//TODO(ches) set up shader
	}

	void render(Scene& scene);

private:
	Shader* shader;
	StageResource<Framebuffer>* const scene_texture;
	StageResource<QuadMesh>* const quad_mesh;
};