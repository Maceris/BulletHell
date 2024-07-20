#pragma once

#include "glm/mat4x4.hpp"

#include "graphics/backend/base/render_stage.h"
#include "graphics/frontend/shader.h"
#include "graphics/scene/sky_box.h"

class SkyboxRender : RenderStage
{
public:
	SkyboxRender(Shader* shader,
		SkyBox* skybox)
		: shader{ shader }
		, skybox{ skybox }
	{}

	void render(Scene& scene);

private:
	glm::mat4 view_matrix;
	const Shader* shader;
	const SkyBox* skybox;
};
