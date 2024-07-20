#pragma once

#include "globals.h"

#include "graphics/glad_types.h"

/// <summary>
/// Used to represent ImGui meshes.
/// </summary>
struct GuiMesh
{
	/// <summary>
	/// The indices Vertex Buffer Object for this mesh.
	/// </summary>
	GLuint indices_VBO;

	/// <summary>
	/// The Vertex Array Object for this mesh.
	/// </summary>
	GLuint VAO;

	/// <summary>
	/// The vertices Vertex Buffer Object for this mesh.
	/// </summary>
	GLuint vertices_VBO;

	/// <summary>
	/// Set up a new mesh.
	/// </summary>
	GuiMesh();

	GuiMesh(const GuiMesh&) = delete;
	GuiMesh& operator=(const GuiMesh&) = delete;
	~GuiMesh();
};