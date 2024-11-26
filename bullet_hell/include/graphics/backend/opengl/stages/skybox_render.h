#pragma once

#include <memory>

#include "glm/mat4x4.hpp"

#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"
#include "graphics/scene/sky_box.h"

class SkyboxRender : public RenderStage
{
public:
	SkyboxRender(StageResource<SkyBox>* skybox);
	virtual ~SkyboxRender();

	virtual void render(Scene& scene);

private:
	struct Data;

	glm::mat4 view_matrix;
	Shader* shader;
	StageResource<SkyBox>* const skybox;
	const std::unique_ptr<Data> data;
};
