#include "GuiMesh.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

GuiMesh::GuiMesh()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &vertices_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, false, ImDrawData)
}