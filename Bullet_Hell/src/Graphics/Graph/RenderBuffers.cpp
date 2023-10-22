#include "RenderBuffers.h"

#include <cmath>
#include <memory>

#include "gtc/type_ptr.hpp"

RenderBuffers::RenderBuffers()
	: vbo_list(std::vector<GLuint>())
	, animated_vao(0)
	, static_vao(0)
	, binding_poses_vbo(0)
	, bones_indices_weights_vbo(0)
	, bones_matrices_vbo(0)
	, dest_animation_vbo(0)
	, buffers_populated(false)
{}

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

	glDeleteBuffers(vbo_list.size(), vbo_list.data());
	vbo_list.clear();
	glDeleteVertexArrays(1, &static_vao);
	glDeleteVertexArrays(1, &animated_vao);
}


void RenderBuffers::define_vertex_attributes()
{
	const int stride = (3 * 4 + 2) * 4;
	int pointer = 0;

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

void RenderBuffers::load_animated_models(const Scene& scene)
{
	buffers_populated = true;
	ModelList model_list;
	for (auto& entry : scene.model_map)
	{
		if (entry.second->is_animated())
		{
			model_list.push_back(entry.second);
		}
	}
	load_binding_poses(model_list);
	load_bones_matrices_buffer(model_list);
	load_bones_indices_weights(model_list);

	glGenVertexArrays(1, &animated_vao);
	glBindVertexArray(animated_vao);

	int vertices_size = 0;
	int offset = 0;
	int mesh_binding_pose_offset = 0;
	int binding_pose_offset = 0;
	int mesh_weights_offset = 0;
	int weights_offset = 0;
	for (auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		std::vector<MeshDrawData>& mesh_draw_data_list 
			= model->mesh_draw_data_list;

		mesh_draw_data_list.clear();
		for (auto& entity : entities)
		{
			binding_pose_offset = mesh_binding_pose_offset;
			weights_offset = mesh_weights_offset;
			for (auto& mesh_data : model->mesh_data_list)
			{
				vertices_size += mesh_data.vertices.size();
				const int mesh_size_in_bytes = sizeof(MeshVertex)
					* mesh_data.vertices.size();

				mesh_draw_data_list.emplace_back(mesh_size_in_bytes,
					mesh_data.material->material_id, offset,
					mesh_data.indices.size(), AnimMeshDrawData(
						entity, binding_pose_offset, weights_offset));

				binding_pose_offset += mesh_size_in_bytes / 4;
				const int group_size = 
					ceil((float) mesh_size_in_bytes / (14 * 4));
				weights_offset += group_size * 2 * 4;
				offset = vertices_size / 3;
			}
		}
		mesh_binding_pose_offset += binding_pose_offset;
		mesh_weights_offset += weights_offset;
	}

	glGenBuffers(1, &dest_animation_vbo);
	vbo_list.push_back(dest_animation_vbo);
	std::vector<float> meshes_buffer;

	GLuint index_vbo = 0;
	glGenBuffers(1, &index_vbo);
	vbo_list.push_back(index_vbo);
	std::vector<float> indices_buffer;

	for (auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		std::vector<MeshData>& mesh_data_list = model->mesh_data_list;
		for (auto& entity : entities)
		{
			for (auto& mesh_data : mesh_data_list)
			{
				mesh_data.append_vertices_to_buffer(meshes_buffer);
				indices_buffer.insert(indices_buffer.end(),
					std::begin(mesh_data.indices),
					std::end(mesh_data.indices));
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, dest_animation_vbo);
	glBufferData(GL_ARRAY_BUFFER, meshes_buffer.size(), meshes_buffer.data(),
		GL_STATIC_DRAW);

	define_vertex_attributes();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_buffer.size(),
		indices_buffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void RenderBuffers::load_binding_poses(const ModelList& models)
{
	glGenBuffers(1, &binding_poses_vbo);
	vbo_list.push_back(binding_poses_vbo);

	std::vector<float> meshes_buffer;
	for (auto& model : models)
	{
		for (auto& mesh_data : model->mesh_data_list)
		{
			mesh_data.append_vertices_to_buffer(meshes_buffer);
		}
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binding_poses_vbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, meshes_buffer.size(), 
		meshes_buffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RenderBuffers::load_bones_indices_weights(const ModelList& models)
{
	std::vector<float> data_buffer;

	for (auto& model : models)
	{
		for (auto& mesh_data : model->mesh_data_list)
		{
			mesh_data.append_weights_to_buffer(data_buffer);
		}
	}

	glGenBuffers(1, &bones_indices_weights_vbo);
	vbo_list.push_back(bones_indices_weights_vbo);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bones_indices_weights_vbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_buffer.size(),
		data_buffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RenderBuffers::load_bones_matrices_buffer(const ModelList& models)
{
	const int matrix_size = 4 * 4;
	std::vector<float> data_buffer;

	for (auto& model : models)
	{
		auto& animation = model->current_animation;
		if (animation)
		{
			for (auto& frame : animation->frames)
			{
				for (auto& bone_matrix : frame.bone_matrices)
				{
					data_buffer.insert(data_buffer.end(), matrix_size,
						*glm::value_ptr(bone_matrix));
				}
			}
		}
	}

	glGenBuffers(1, &bones_matrices_vbo);
	vbo_list.push_back(bones_matrices_vbo);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bones_matrices_vbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_buffer.size(),
		data_buffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void RenderBuffers::load_static_models(const Scene& scene)
{
	buffers_populated = true;
	ModelList model_list;
	for (auto& entry : scene.model_map)
	{
		if (!entry.second->is_animated())
		{
			model_list.push_back(entry.second);
		}
	}
	load_binding_poses(model_list);
	load_bones_matrices_buffer(model_list);
	load_bones_indices_weights(model_list);

	glGenVertexArrays(1, &static_vao);
	glBindVertexArray(static_vao);

	int vertices_size = 0;
	int offset = 0;

	for (auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		std::vector<MeshDrawData>& mesh_draw_data_list
			= model->mesh_draw_data_list;

		mesh_draw_data_list.clear();
		for (auto& entity : entities)
		{
			for (MeshData mesh_data : model->mesh_data_list)
			{
				vertices_size += mesh_data.vertices.size();
				const int mesh_size_in_bytes = sizeof(MeshVertex)
					* mesh_data.vertices.size();

				mesh_draw_data_list.emplace_back(mesh_size_in_bytes,
					mesh_data.material->material_id, offset,
					mesh_data.indices.size());
				
				offset = vertices_size / 3;
			}
		}
	}

	GLuint meshes_vbo = 0;
	glGenBuffers(1, &meshes_vbo);
	vbo_list.push_back(meshes_vbo);
	std::vector<float> meshes_buffer;

	GLuint index_vbo = 0;
	glGenBuffers(1, &index_vbo);
	vbo_list.push_back(index_vbo);
	std::vector<float> indices_buffer;

	for (auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		std::vector<MeshData>& mesh_data_list = model->mesh_data_list;
		for (auto& entity : entities)
		{
			for (auto& mesh_data : mesh_data_list)
			{
				mesh_data.append_vertices_to_buffer(meshes_buffer);
				indices_buffer.insert(indices_buffer.end(),
					std::begin(mesh_data.indices),
					std::end(mesh_data.indices));
			}
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, meshes_vbo);
	glBufferData(GL_ARRAY_BUFFER, meshes_buffer.size(), meshes_buffer.data(),
		GL_STATIC_DRAW);

	define_vertex_attributes();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_buffer.size(),
		indices_buffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


