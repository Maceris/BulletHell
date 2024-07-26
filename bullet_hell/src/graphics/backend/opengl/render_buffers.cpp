#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL || BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/backend/opengl/render_buffers.h"

#include <cmath>
#include <cstdint>
#include <memory>
#include <set>

#include "glm/gtc/type_ptr.hpp"

#include "debugging/logger.h"
#include "graphics/graph/animation_resource.h"
#include "graphics/graph/material.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/graph/mesh_draw_data.h"
#include "graphics/scene/scene.h"

#include "glad.h"

RenderBuffers::RenderBuffers()
	: vbo_list{}
	, buffers_populated{ false }
{
	glGenVertexArrays(1, &static_vao);
	glGenBuffers(1, &static_meshes_vbo);
	vbo_list.push_back(static_meshes_vbo);
	glGenBuffers(1, &static_index_vbo);
	vbo_list.push_back(static_index_vbo);

	glGenVertexArrays(1, &animated_vao);
	glGenBuffers(1, &dest_animation_vbo);
	vbo_list.push_back(dest_animation_vbo);
	glGenBuffers(1, &animated_index_vbo);
	vbo_list.push_back(animated_index_vbo);
	glGenBuffers(1, &bones_matrices_ssbo);
	vbo_list.push_back(bones_matrices_ssbo);
	glGenBuffers(1, &bones_indices_weights_ssbo);
	vbo_list.push_back(bones_indices_weights_ssbo);
	glGenBuffers(1, &binding_poses_ssbo);
	vbo_list.push_back(binding_poses_ssbo);
	glGenBuffers(1, &animation_draw_parameters_ssbo);
	vbo_list.push_back(animation_draw_parameters_ssbo);
}

RenderBuffers::~RenderBuffers()
{
	cleanup();
}

void RenderBuffers::cleanup()
{
	if (!buffers_populated)
	{
		return;
	}
	buffers_populated = false;

	glDeleteBuffers(static_cast<int>(vbo_list.size()), vbo_list.data());
	vbo_list.clear();
	glDeleteVertexArrays(1, &static_vao);
	glDeleteVertexArrays(1, &animated_vao);
}

void RenderBuffers::define_vertex_attributes()
{
	const int stride = (3 * 4 + 2) * (int) sizeof(float);
	uintptr_t pointer = 0;

	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)pointer);
	pointer += 3 * 4;

	// Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)pointer);
	pointer += 3 * 4;

	// Tangents
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, stride, (void*)pointer);
	pointer += 3 * 4;

	// Bitangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, stride, (void*)pointer);
	pointer += 3 * 4;

	// Texture coordinates
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, false, stride, (void*)pointer);
}

void RenderBuffers::load_animated_entity_buffers(const Scene& scene)
{
	buffers_populated = true;

	const ModelList& model_list = scene.get_animated_model_list();
	glBindVertexArray(animated_vao);

	size_t vertices_size = 0;
	size_t offset = 0;
	size_t mesh_binding_pose_offset = 0;
	size_t binding_pose_offset = 0;
	size_t mesh_weights_offset = 0;
	size_t weights_offset = 0;
	for (auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		std::vector<MeshDrawData>& mesh_draw_data_list
			= model->mesh_draw_data_list;

		mesh_draw_data_list.clear();
		if (model->entity_list.empty())
		{
			continue;
		}
		for (auto& entity : entities)
		{
			binding_pose_offset = mesh_binding_pose_offset;
			weights_offset = mesh_weights_offset;
			for (auto& mesh_data : model->mesh_data_list)
			{
				vertices_size += mesh_data.vertices.size();
				const size_t mesh_size_in_bytes = sizeof(MeshVertex)
					* mesh_data.vertices.size();

				mesh_draw_data_list.emplace_back(mesh_size_in_bytes,
					mesh_data.material->material_id, offset,
					mesh_data.indices.size(), AnimMeshDrawData(
						entity,
						static_cast<int>(binding_pose_offset),
						static_cast<int>(weights_offset)
					)
				);

				binding_pose_offset += mesh_size_in_bytes / sizeof(float);
				const size_t group_size = mesh_data.vertices.size();
				weights_offset += group_size * 2 * sizeof(float);
				offset = vertices_size;
			}
		}
		mesh_binding_pose_offset += binding_pose_offset;
		mesh_weights_offset += weights_offset;
	}

	std::vector<float> meshes_buffer;
	std::vector<uint32_t> indices_buffer;

	LOG_ASSERT(sizeof(GLuint) == sizeof(float)
		&& "This system has different sizes for unsigned int and float, which is currently unsupported.");

	for (auto& model : model_list)
	{
		const std::vector<MeshData>& mesh_data_list = model->mesh_data_list;
		for (const auto& entity : model->entity_list)
		{
			for (const auto& mesh_data : mesh_data_list)
			{
				mesh_data.append_vertices_to_buffer(meshes_buffer);
				mesh_data.append_indices_to_buffer(indices_buffer);
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, dest_animation_vbo);
	glBufferData(GL_ARRAY_BUFFER, meshes_buffer.size() * sizeof(float),
		meshes_buffer.data(), GL_DYNAMIC_DRAW);

	define_vertex_attributes();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, animated_index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices_buffer.size() * sizeof(uint32_t), indices_buffer.data(),
		GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glBindVertexArray(0);
}

void RenderBuffers::load_animated_models(const Scene& scene)
{
	buffers_populated = true;
	const ModelList& model_list = scene.get_animated_model_list();

	load_binding_poses(model_list);
	load_bones_matrices_buffer(model_list);
	load_bones_indices_weights(model_list);
}

void RenderBuffers::load_binding_poses(const ModelList& models)
{
	std::vector<float> meshes_buffer;
	for (const auto& model : models)
	{
		if (model->entity_list.empty())
		{
			continue;
		}
		for (const auto& mesh_data : model->mesh_data_list)
		{
			mesh_data.append_vertices_to_buffer(meshes_buffer);
		}
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binding_poses_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 
		meshes_buffer.size() * sizeof(float), meshes_buffer.data(),
		GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RenderBuffers::load_bones_indices_weights(const ModelList& models)
{
	std::vector<float> data_buffer;

	for (auto& model : models)
	{
		if (model->entity_list.empty())
		{
			continue;
		}
		for (auto& mesh_data : model->mesh_data_list)
		{
			mesh_data.append_weights_to_buffer(data_buffer);
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bones_indices_weights_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_buffer.size() * sizeof(float),
		data_buffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RenderBuffers::load_bones_matrices_buffer(const ModelList& models)
{
	const int matrix_size = 4 * 4;
	std::vector<float> data_buffer;

	int current_offset = 0;
	for (auto& model : models)
	{
		if (model->entity_list.empty())
		{
			continue;
		}
		std::set<std::shared_ptr<Animation>> animation_list;

		for (auto& animation : model->animation_list)
		{
			animation_list.insert(animation);
		}

		for (auto& animation: animation_list)
		{
			for (auto& frame : animation->frames)
			{
				frame.offset = current_offset;
				for (auto& bone_matrix : frame.bone_matrices)
				{
					for (int col = 0; col < 4; ++col)
					{
						for (int row = 0; row < 4; ++row)
						{
							data_buffer.push_back(bone_matrix[col][row]);
						}
					}
				}
				current_offset += static_cast<int>(frame.bone_matrices.size());
			}
		}
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bones_matrices_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_buffer.size() * sizeof(float),
		data_buffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void RenderBuffers::load_static_models(const Scene& scene)
{
	buffers_populated = true;
	const std::vector<std::shared_ptr<Model>>& model_list =
		scene.get_static_model_list();
	
	glBindVertexArray(static_vao);

	size_t vertices_size = 0;
	size_t offset = 0;

	for (auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		std::vector<MeshDrawData>& mesh_draw_data_list
			= model->mesh_draw_data_list;

		mesh_draw_data_list.clear();
		for (MeshData mesh_data : model->mesh_data_list)
		{
			vertices_size += mesh_data.vertices.size();
			const size_t mesh_size_in_bytes = sizeof(MeshVertex)
				* mesh_data.vertices.size();

			mesh_draw_data_list.emplace_back(mesh_size_in_bytes,
				mesh_data.material->material_id, offset,
				mesh_data.indices.size());
				
			offset = vertices_size;
		}
	}
	
	std::vector<float> meshes_buffer;
	std::vector<uint32_t> indices_buffer;

	LOG_ASSERT(sizeof(GLuint) == sizeof(float)
	&& "This system has different sizes for unsigned int and float, which is currently unsupported.");

	for (auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		std::vector<MeshData>& mesh_data_list = model->mesh_data_list;
		
		for (auto& mesh_data : mesh_data_list)
		{
			mesh_data.append_vertices_to_buffer(meshes_buffer);
			mesh_data.append_indices_to_buffer(indices_buffer);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, static_meshes_vbo);
	glBufferData(GL_ARRAY_BUFFER, meshes_buffer.size() * sizeof(float), 
		meshes_buffer.data(), GL_STATIC_DRAW);

	define_vertex_attributes();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
		indices_buffer.size() * sizeof(uint32_t), indices_buffer.data(),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#endif
