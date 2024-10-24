#pragma once

#include <string_view>

#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

class FilterRender : public RenderStage
{
public:
	FilterRender(StageResource<Framebuffer>* scene_texture,
		StageResource<QuadMesh>* quad_mesh);
	virtual ~FilterRender() = default;

	virtual void render(Scene& scene);

private:
	Shader* shader;
	StageResource<Framebuffer>* const scene_texture;
	StageResource<QuadMesh>* const quad_mesh;

	void set_filter(const std::string_view name);
};