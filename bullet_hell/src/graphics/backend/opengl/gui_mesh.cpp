#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/gui_mesh.h"

#include "glad.h"
#include "graphics/gui/ui.h"

GuiMesh::GuiMesh()
	: indices_VBO{ 0 }
	, VAO{ 0 }
	, vertices_VBO{ 0 }
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glEnableVertexAttribArray(0);
	const int sizeof_draw_vert = sizeof(ImDrawVert);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof_draw_vert, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof_draw_vert, 
		(const void*) 8);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, false, sizeof_draw_vert, 
		(const void*) 16);

	glGenBuffers(1, &indices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

GuiMesh::~GuiMesh()
{
	glDeleteBuffers(1, &indices_VBO);
	glDeleteBuffers(1, &vertices_VBO);
	glDeleteVertexArrays(1, &VAO);
}

#endif
