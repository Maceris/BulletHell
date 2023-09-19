#pragma once

#include "Globals.h"

#include "glad.h"


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

	/// <summary>
	/// We don't copy the meshes.
	/// </summary>
	GuiMesh(const GuiMesh&) = delete;

	/// <summary>
	/// We don't copy the meshes.
	/// </summary>
	GuiMesh& operator=(const GuiMesh&) = delete;

	/// <summary>
	/// Clean up the data.
	/// </summary>
	~GuiMesh();
};