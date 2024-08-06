#pragma once

#include "glm/mat4x4.hpp"

#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"
#include "graphics/scene/sky_box.h"

class SkyboxRender : RenderStage
{
public:
	SkyboxRender(StageResource<SkyBox>* skybox);

	void render(Scene& scene);

private:
	glm::mat4 view_matrix;
	Shader* shader;
	StageResource<SkyBox>* const skybox;
};
