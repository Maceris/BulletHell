#pragma once

#include "Globals.h"

#include <vector>

#include "glad.h"
#include "glm.hpp"
#include "MeshData.h"

class Mesh
{

private:
	/// <summary>
	/// The number of VBO's that we use. These represent:
	/// 1) positions
	/// 2) normals
	/// 3) tangents
	/// 4) bitangents
	/// 5) texture coordinates
	/// 6) bone weights
	/// 7) bone indices
	/// 8) indices
	/// </summary>
	static const int VBO_COUNT = 8;
public:
	/// <summary>
	/// The min value (corner) of the axis-aligned bounding box for this mesh.
	/// </summary>
	glm::vec3 aabb_max;

	/// <summary>
	/// The max value (corner) of the axis-aligned bounding box for this mesh.
	/// </summary>
	glm::vec3 aabb_min;

	/// <summary>
	/// The number of vertices in the mesh.
	/// </summary>
	int vertex_count;

	/// <summary>
	/// The OpenGL vertex array object ID.
	/// </summary>
	GLuint vao;

	/// <summary>
	/// The list of vertex buffer objects for this mesh.
	/// </summary>
	GLuint vbo_list[VBO_COUNT];

	/// <summary>
	/// Create a mesh from prepared mesh data.
	/// </summary>
	/// <param name="mesh_data">The mesh data to load in.</param>
	Mesh(MeshData& mesh_data);

	/// <summary>
	/// We don't copy meshes.
	/// </summary>
	Mesh(const Mesh&) = delete;

	/// <summary>
	/// We don't copy meshes.
	/// </summary>
	Mesh& operator=(const Mesh&) = delete;

	/// <summary>
	/// Clean up data.
	/// </summary>
	~Mesh();

};