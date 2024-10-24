#pragma once

#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/frontend/render_stage.h"

class ModelMatrixUpdate : public RenderStage
{
public:
	ModelMatrixUpdate(StageResource<CommandBuffers>* command_buffers)
		: command_buffers{ command_buffers }
	{}
	virtual ~ModelMatrixUpdate() = default;

	virtual void render(Scene& scene);

private:
	StageResource<CommandBuffers>* const command_buffers;
};