#pragma once

#include "graphics/backend/base/render_stage.h"
#include "graphics/backend/opengl/command_buffers.h"

class ModelMatrixUpdate : RenderStage
{
public:
	ModelMatrixUpdate(CommandBuffers* command_buffers)
		: command_buffers{ command_buffers }
	{}

	void render(Scene& scene);

private:
	CommandBuffers* const command_buffers;
};