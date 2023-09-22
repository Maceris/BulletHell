#include "SkyBox.h"

SkyBox::SkyBox()
{

}

void SkyBox::populate_buffers(const MeshData& mesh_data)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(VBO_COUNT, vbo_list);

	const GLuint vbo_positions = vbo_list[0];
	const GLuint vbo_texture_coordinates = vbo_list[1];
	const GLuint vbo_indices = vbo_list[2];

	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.positions.size(),
		mesh_data.positions.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_coordinates);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.texture_coordinates.size(),
		mesh_data.texture_coordinates.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data.indices.size(),
		mesh_data.indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

SkyBox::~SkyBox()
{
	glDeleteBuffers(VBO_COUNT, vbo_list);
	glDeleteVertexArrays(1, &vao);
}