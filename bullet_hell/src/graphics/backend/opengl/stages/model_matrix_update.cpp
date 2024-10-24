#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "glm/gtc/type_ptr.hpp"

#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/backend/opengl/stages/model_matrix_update.h"
#include "graphics/scene/scene.h"
#include "memory/memory_util.h"

#include "glad.h"

void ModelMatrixUpdate::render(Scene& scene)
{
	ModelList animated_models = scene.get_animated_model_list();
	ModelList static_models = scene.get_static_model_list();

	GLuint animated_buffer = (*command_buffers)->animated_model_matrices_buffer;
	GLuint static_buffer = (*command_buffers)->static_model_matrices_buffer;

	update_model_buffer(animated_models, animated_buffer);
	update_model_buffer(static_models, static_buffer);
}

void ModelMatrixUpdate::update_model_buffer(
	const std::vector<std::shared_ptr<Model>> models, GLuint buffer_id)
{
	size_t entity_count = 0;
	for (const auto& model : models)
	{
		entity_count += model->entity_list.size();
	}

	float* model_matrices = ALLOC float[entity_count * 16];

	int entity_index = 0;
	for (const auto& model : models)
	{
		for (const auto& entity : model->entity_list)
		{
			const float* matrix = static_cast<const float*>(
				glm::value_ptr(entity->model_matrix));
			for (size_t i = 0; i < 16; ++i)
			{
				model_matrices[static_cast<size_t>(entity_index) * 16 + i]
					= matrix[i];
			}
			++entity_index;
		}
	}
	size_t data_size_in_bytes = entity_count * 16 * sizeof(float);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes,
		model_matrices, GL_DYNAMIC_DRAW);

	safe_delete_array(model_matrices);
}

#endif
