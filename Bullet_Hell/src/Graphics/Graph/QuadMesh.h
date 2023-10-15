#pragma once

#include "Globals.h"

#include "glad.h"

constexpr auto QUAD_MESH_VERTEX_COUNT = 6;

/// <summary>
/// Defines a quad that is used to render in the lighting pass.
/// </summary>
struct QuadMesh
{
	/// <summary>
	/// The Vertex Array Object ID for the mesh.
	/// </summary>
	GLuint vao;

	/// <summary>
	/// The list of VBOs created for the mesh. These represent:
	/// 1) positions
	/// 2) texture coordinates
	/// 3) indices
	/// </summary>
	GLuint vbo_list[3];

	/// <summary>
	/// Set up a new mesh, which forms a quad from (-1, -1) to (1, 1).
	/// </summary>
	QuadMesh();

	QuadMesh(const QuadMesh&) = delete;
	QuadMesh& operator=(const QuadMesh&) = delete;
	~QuadMesh();
};