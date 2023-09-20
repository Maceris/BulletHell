#include "Mesh.h"

Mesh::Mesh(MeshData& mesh_data)
	: aabb_min(mesh_data.aabb_min)
	, aabb_max(mesh_data.aabb_max)
	, vertex_count(mesh_data.indices.size())
	, vao{ 0 }
	, vbo_list{ 0 }
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(VBO_COUNT, vbo_list);

	const GLuint vbo_positions           = vbo_list[0];
	const GLuint vbo_normals             = vbo_list[1];
	const GLuint vbo_tangents            = vbo_list[2];
	const GLuint vbo_bitangents          = vbo_list[3];
	const GLuint vbo_texture_coordinates = vbo_list[4];
	const GLuint vbo_bone_weights        = vbo_list[5];
	const GLuint vbo_bone_indices        = vbo_list[6];
	const GLuint vbo_indices             = vbo_list[7];

	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.positions.size(), 
		mesh_data.positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.normals.size(), 
		mesh_data.normals.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_tangents);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.tangents.size(),
		mesh_data.tangents.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_bitangents);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.bitangents.size(),
		mesh_data.bitangents.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_coordinates);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.texture_coordinates.size(),
		mesh_data.texture_coordinates.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_bone_weights);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.weights.size(),
		mesh_data.weights.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, MeshData::MAX_WEIGHTS, GL_FLOAT, false, 0,
		nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_bone_indices);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.bone_indices.size(),
		mesh_data.bone_indices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_INT, false, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data.indices.size(),
		mesh_data.indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::~Mesh()
{
	glDeleteBuffers(VBO_COUNT, vbo_list);
	glDeleteVertexArrays(1, &vao);
}