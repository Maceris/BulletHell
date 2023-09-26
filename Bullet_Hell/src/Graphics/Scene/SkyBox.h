#pragma once

#include "Globals.h"

#include <memory>

#include "Entity.h"
#include "Material.h"
#include "Model.h"

class SkyBox
{
private:
	/// <summary>
	/// The number of VBO's that we use. These represent:
	/// 1) positions
	/// 2) texture coordinates
	/// 3) indices
	/// </summary>
	static const int VBO_COUNT = 3;

	/// <summary>
	/// Load the mesh data into buffers for rendering.
	/// </summary>
	/// <param name="mesh_data">The data to load.</param>
	void populate_buffers(const MeshData& mesh_data);

public:

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

	material_id material;

	std::shared_ptr<Entity> entity;
	std::shared_ptr<Model> model;
	SkyBox();//TODO(ches) model loading setup (model path?)
	SkyBox(const SkyBox&) = delete;
	SkyBox& operator=(const SkyBox&) = delete;
	~SkyBox();
};