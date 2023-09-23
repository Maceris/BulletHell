#pragma once

#include "Globals.h"

#include "glad.h"

/// <summary>
/// Defines a quad that is used to render in the lighting pass.
/// </summary>
class QuadMesh
{
private:
	/// <summary>
	/// The number of VBO's that we use. These represent:
	/// 1) positions
	/// 2) texture coordinates
	/// 3) indices
	/// </summary>
	static const int VBO_COUNT = 3;
public:
	/// <summary>
	/// The number of vertices the mesh has.
	/// </summary>
	const int vertex_count = 6;

	/// <summary>
	/// The Vertex Array Object ID for the mesh.
	/// </summary>
	GLuint vao;

	/// <summary>
	/// The list of VBOs created for the mesh.
	/// </summary>
	GLuint vbo_list[VBO_COUNT];

	/// <summary>
	/// Set up a new mesh, which forms a quad from (-1, -1) to (1, 1).
	/// </summary>
	QuadMesh();

	/// <summary>
	/// We don't copy meshes.
	/// </summary>
	QuadMesh(const QuadMesh&) = delete;

	/// <summary>
	/// We don't copy meshes.
	/// </summary>
	QuadMesh& operator=(const QuadMesh&) = delete;

	/// <summary>
	/// Cleanup data.
	/// </summary>
	~QuadMesh();
};