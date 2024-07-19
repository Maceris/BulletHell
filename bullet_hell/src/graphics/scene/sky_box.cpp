#include "graphics/scene/sky_box.h"

#include "debugging/logger.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/graph/model.h"
#include "graphics/graph/model_resource.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

#include "glad.h"

SkyBox::SkyBox()
{
	const std::string model_name = "models/skybox/skybox.model";
	model = load_model(model_name);
	entity = std::make_shared<Entity>(model_name);

	LOG_ASSERT(model->mesh_data_list.size() == 1
		&& "We are assuming that skybox models only have one mesh");
	index_count = (int) model->mesh_data_list[0].indices.size();
	populate_buffers(model->mesh_data_list[0]);
}

void SkyBox::populate_buffers(const MeshData& mesh_data)
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(SKYBOX_VBO_COUNT, vbo_list);

	const GLuint vbo_positions = vbo_list[0];
	const GLuint vbo_texture_coordinates = vbo_list[1];
	const GLuint vbo_indices = vbo_list[2];

	const size_t positions_size = mesh_data.vertices.size() * 3;
	const size_t texture_coordinates_size = mesh_data.vertices.size() * 2;

	float* positions = ALLOC float[positions_size];
	float* texture_coordinates = ALLOC float[texture_coordinates_size];

	size_t index = 0;
	for (const MeshVertex& vertex : mesh_data.vertices)
	{
		positions[index * 3 + 0] = vertex.position[0];
		positions[index * 3 + 1] = vertex.position[1];
		positions[index * 3 + 2] = vertex.position[2];

		texture_coordinates[index * 2 + 0] = vertex.texture_coordinates[0];
		texture_coordinates[index * 2 + 1] = vertex.texture_coordinates[1];
		++index;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, positions_size * sizeof(float), positions,
		GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_coordinates);
	glBufferData(GL_ARRAY_BUFFER, texture_coordinates_size * sizeof(float),
		texture_coordinates, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, nullptr);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data.indices.size() 
		* sizeof(uint32_t), mesh_data.indices.data(), GL_STATIC_DRAW);

	SAFE_DELETE_ARRAY(positions);
	SAFE_DELETE_ARRAY(texture_coordinates);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

SkyBox::~SkyBox()
{
	glDeleteBuffers(SKYBOX_VBO_COUNT, vbo_list);
	glDeleteVertexArrays(1, &vao);
}