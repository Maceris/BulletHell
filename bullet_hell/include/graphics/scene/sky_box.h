#pragma once

#include <memory>

#include "graphics/glad_types.h"

struct Entity;
struct MeshData;
struct Model;

constexpr auto SKYBOX_VBO_COUNT = 3;

class SkyBox
{
public:
	SkyBox();
	SkyBox(const SkyBox&) = delete;
	SkyBox& operator=(const SkyBox&) = delete;
	~SkyBox();

	/// <summary>
	/// Load the mesh data into buffers for rendering.
	/// </summary>
	/// <param name="mesh_data">The data to load.</param>
	void populate_buffers(const MeshData& mesh_data);

	/// <summary>
	/// The number of indices in the mesh.
	/// </summary>
	int index_count;

	/// <summary>
	/// The OpenGL vertex array object ID.
	/// </summary>
	GLuint vao;

	/// <summary>
	/// The list of vertex buffer objects for this mesh. These represent:
	/// 1) positions
	/// 2) texture coordinates
	/// 3) indices
	/// </summary>
	GLuint vbo_list[SKYBOX_VBO_COUNT];

	std::shared_ptr<Entity> entity;
	std::shared_ptr<Model> model;
};