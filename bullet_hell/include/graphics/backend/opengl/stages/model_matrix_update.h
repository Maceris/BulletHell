#pragma once

#include <memory>
#include <vector>

#include "graphics/glad_types.h"
#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/graph/model.h"

class ModelMatrixUpdate : public RenderStage
{
public:
	ModelMatrixUpdate(StageResource<CommandBuffers>* command_buffers)
		: command_buffers{ command_buffers }
	{}
	virtual ~ModelMatrixUpdate() = default;

	virtual void render(Scene& scene);

	/// <summary>
	/// Take a list of models, and upload the model matrices to the specified 
	/// buffer.
	/// </summary>
	/// <param name="models">The list of models.</param>
	/// <param name="buffer_id">The buffer we want to send data to.</param>
	void update_model_buffer(const std::vector<std::shared_ptr<Model>> models,
		GLuint buffer_id);

private:
	StageResource<CommandBuffers>* const command_buffers;
};