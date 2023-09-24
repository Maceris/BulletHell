#include "RenderBuffers.h"

#include <cmath>

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
	glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void *) pointer);
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
	for (auto entry : scene.model_map)
	{
		if (entry.second.get()->is_animated())
		{
			model_list.push_back(entry.second);
		}
	}
	load_binding_poses(model_list);
	load_bones_matrices_buffer(model_list);
	load_bones_indices_weights(model_list);

	glGenVertexArrays(1, &animated_vao);
	glBindVertexArray(animated_vao);

	int positions_size = 0;
	int normals_size = 0;
	int texture_coordinates_size = 0;
	int indices_size = 0;
	int offset = 0;
	int chunk_binding_pose_offset = 0;
	int binding_pose_offset = 0;
	int chunk_weights_offset = 0;
	int weights_offset = 0;
	for (auto model : model_list)
	{
		EntityList& entities = model.get()->entity_list;
		std::vector<MeshDrawData>& mesh_draw_data_list 
			= model.get()->mesh_draw_data_list;

		mesh_draw_data_list.clear();
		for (auto entity : entities)
		{
			binding_pose_offset = chunk_binding_pose_offset;
			weights_offset = chunk_weights_offset;
			for (MeshData mesh_data : model.get()->mesh_data_list)
			{
				positions_size += mesh_data.positions.size();
				normals_size += mesh_data.normals.size();
				texture_coordinates_size += 
					mesh_data.texture_coordinates.size();
				indices_size += mesh_data.indices.size();

				const int meshSizeInBytes = (mesh_data.positions.size()
					+ mesh_data.normals.size() * 3
					+ mesh_data.texture_coordinates.size()) * 4;

				mesh_draw_data_list.emplace_back(meshSizeInBytes,
					mesh_data.material_id, offset,
					mesh_data.indices.size(), AnimMeshDrawData(
						entity, binding_pose_offset, weights_offset));

				binding_pose_offset += meshSizeInBytes / 4;
				const int groupSize = 
					ceil((float) meshSizeInBytes / (14 * 4));
				weights_offset += groupSize * 2 * 4;
				offset = positions_size / 3;
				
			}
		}
		chunk_binding_pose_offset += binding_pose_offset;
		chunk_weights_offset += weights_offset;
	}

	glGenBuffers(1, &dest_animation_vbo);
	vbo_list.push_back(dest_animation_vbo);
	std::vector<float> meshes_buffer(
		positions_size + normals_size * 3 + texture_coordinates_size);

	GLuint index_vbo = 0;
	glGenBuffers(1, &index_vbo);
	vbo_list.push_back(index_vbo);
	std::vector<float> indices_buffer(indices_size);

	for (auto model : model_list)
	{
		EntityList& entities = model.get()->entity_list;
		std::vector<MeshData>& mesh_data_list = model.get()->mesh_data_list;
		for (auto entity : entities)
		{
			for (auto mesh_data : mesh_data_list)
			{
				populate_mesh_buffer(meshes_buffer, mesh_data);
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
	int mesh_size = 0;
	for (auto model : models)
	{
		for (auto mesh_data : model.get()->mesh_data_list)
		{
			mesh_size += mesh_data.positions.size()
				+ mesh_data.normals.size() * 3
				+ mesh_data.texture_coordinates.size()
				+ mesh_data.indices.size();
		}
	}

	glGenBuffers(1, &binding_poses_vbo);
	vbo_list.push_back(binding_poses_vbo);

	std::vector<float> meshes_buffer(mesh_size);
	for (auto model : models)
	{
		for (auto mesh_data : model.get()->mesh_data_list)
		{
			populate_mesh_buffer(meshes_buffer, mesh_data);
		}
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, binding_poses_vbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, meshes_buffer.size(), 
		meshes_buffer.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void RenderBuffers::load_bones_indices_weights(const ModelList& models)
{
	int buffer_size = 0;
	for (auto model : models)
	{
		for (auto mesh_data : model.get()->mesh_data_list)
		{
			buffer_size += mesh_data.bone_indices.size() * 4
				+ mesh_data.weights.size() * 4;
		}
	}

	std::vector<float> data_buffer(buffer_size);

	for (auto model : models)
	{
		for (auto mesh_data : model.get()->mesh_data_list)
		{
			std::vector<int>& bone_indices = mesh_data.bone_indices;
			std::vector<float>& weights = mesh_data.weights;
			const int entries = bone_indices.size() / 4;
			for (int i = 0; i < entries; ++i)
			{
				data_buffer.insert(data_buffer.end(), std::begin(bone_indices),
					std::end(bone_indices));
				data_buffer.insert(data_buffer.end(), std::begin(weights),
					std::end(weights));

			}
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
	const int matrix_size = 4 * 4 * 4;
	int buffer_size = 0;
	for (auto model : models)
	{
		for (auto animation : model.get()->animation_list)
		{
			for (auto frame : animation.frames)
			{
				buffer_size += frame.bones_matrices.size() * matrix_size;
			}
		}
	}

	std::vector<float> data_buffer(buffer_size);

	// TODO don't have a buck wild cognitive complexity
	for (auto model : models)
	{
		for (auto animation : model.get()->animation_list)
		{
			for (auto frame : animation.frames)
			{
				for (glm::mat4 bone_matrix : frame.bones_matrices)
				{
					// glm defaults to column-major order, which we need
					for (int column = 0; column < 4; ++column)
					{
						for (int row = 0; row < 4; ++row)
						{
							data_buffer.push_back(bone_matrix[column][row]);
						}
					}
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
	for (auto entry : scene.model_map)
	{
		if (!entry.second.get()->is_animated())
		{
			model_list.push_back(entry.second);
		}
	}
	load_binding_poses(model_list);
	load_bones_matrices_buffer(model_list);
	load_bones_indices_weights(model_list);

	glGenVertexArrays(1, &animated_vao);
	glBindVertexArray(animated_vao);

	int positions_size = 0;
	int normals_size = 0;
	int texture_coordinates_size = 0;
	int indices_size = 0;
	int offset = 0;

	for (auto model : model_list)
	{
		EntityList& entities = model.get()->entity_list;
		std::vector<MeshDrawData>& mesh_draw_data_list
			= model.get()->mesh_draw_data_list;

		mesh_draw_data_list.clear();
		for (auto entity : entities)
		{
			for (MeshData mesh_data : model.get()->mesh_data_list)
			{
				positions_size += mesh_data.positions.size();
				normals_size += mesh_data.normals.size();
				texture_coordinates_size += 
					mesh_data.texture_coordinates.size();
				indices_size += mesh_data.indices.size();

				const int meshSizeInBytes = 
					mesh_data.positions.size() * 14 * 4;

				mesh_draw_data_list.emplace_back(meshSizeInBytes,
					mesh_data.material_id, offset,
					mesh_data.indices.size());

				offset = positions_size / 3;

			}
		}
	}

	GLuint meshes_vbo = 0;
	glGenBuffers(1, &meshes_vbo);
	vbo_list.push_back(meshes_vbo);
	std::vector<float> meshes_buffer(
		positions_size + normals_size * 3 + texture_coordinates_size);

	GLuint index_vbo = 0;
	glGenBuffers(1, &index_vbo);
	vbo_list.push_back(index_vbo);
	std::vector<float> indices_buffer(indices_size);

	for (auto model : model_list)
	{
		EntityList& entities = model.get()->entity_list;
		std::vector<MeshData>& mesh_data_list = model.get()->mesh_data_list;
		for (auto entity : entities)
		{
			for (auto mesh_data : mesh_data_list)
			{
				populate_mesh_buffer(meshes_buffer, mesh_data);
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

void RenderBuffers::populate_mesh_buffer(
	std::vector<float>& meshes_buffer, const MeshData& mesh_data)
{
	const int entries = mesh_data.positions.size() / 3;
	for (int i = 0; i < entries; ++i)
	{
		meshes_buffer.insert(meshes_buffer.end(), 
			std::begin(mesh_data.positions), std::end(mesh_data.positions));
		meshes_buffer.insert(meshes_buffer.end(),
			std::begin(mesh_data.normals), std::end(mesh_data.normals));
		meshes_buffer.insert(meshes_buffer.end(),
			std::begin(mesh_data.tangents), std::end(mesh_data.tangents));
		meshes_buffer.insert(meshes_buffer.end(),
			std::begin(mesh_data.bitangents), std::end(mesh_data.bitangents));
		meshes_buffer.insert(meshes_buffer.end(),
			std::begin(mesh_data.texture_coordinates), 
			std::end(mesh_data.texture_coordinates));
	}
}

