#include "QuadMesh.h"

#include "glad.h"

QuadMesh::QuadMesh()
	: vao{ 0 }
	, vbo_list{ 0 }
{
	const float positions[] =
	{
		-1.0f,  1.0f, 0.0f, 
		 1.0f,  1.0f, 0.0f, 
		-1.0f, -1.0f, 0.0f, 
		 1.0f, -1.0f, 0.0f
	};
	const float texture_coordinates[] = 
	{
		0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	const int indices[] = {0, 2, 1, 1, 2, 3};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(3, vbo_list);

	const GLuint vbo_positions = vbo_list[0];
	const GLuint vbo_texture_coordinates = vbo_list[1];
	const GLuint vbo_indices = vbo_list[2];

	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, 
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_coordinates);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture_coordinates),
		texture_coordinates, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

QuadMesh::~QuadMesh()
{
	glDeleteBuffers(3, vbo_list);
	glDeleteVertexArrays(1, &vao);
}