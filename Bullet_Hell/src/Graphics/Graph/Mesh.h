#pragma once

#include "Globals.h"

#include <vector>

#include "glad.h"
#include "glm.hpp"
#include "MeshData.h"

class Mesh
{
public:
	/// <summary>
	/// The maximum number of weights that can affect a vertex.
	/// </summary>
	static const int MAX_WEIGHTS = 4;

	glm::vec3 aabb_max;
	glm::vec3 aabb_min;
	int vertex_count;
	GLuint vao;
	std::vector<GLuint> vbo_list;
	Mesh(const MeshData& mesh_data);
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	~Mesh();
};