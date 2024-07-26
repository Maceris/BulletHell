#pragma once

#include "glm/mat4x4.hpp"

#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"
#include "graphics/scene/sky_box.h"

class SkyboxRender : RenderStage
{
public:
	SkyboxRender(Shader* shader,
		StageResource<SkyBox>* skybox)
		: shader{ shader }
		, skybox{ skybox }
	{}

	void render(Scene& scene);

private:
	glm::mat4 view_matrix;
	Shader* const shader;
	StageResource<SkyBox>* const skybox;
};
