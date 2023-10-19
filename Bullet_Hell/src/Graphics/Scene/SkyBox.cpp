#include "SkyBox.h"

#include "GameLogic.h"
#include "Logger.h"
#include "ModelResource.h"
#include "ResourceCache.h"

SkyBox::SkyBox()
{
	Resource model_resource("models/skybox/skybox.model");
	auto model_handle = g_game_logic->resource_cache->
		get_handle(&model_resource);
	
	std::shared_ptr<ModelExtraData> model_extra =
		static_pointer_cast<ModelExtraData>(model_handle->get_extra());
	model = model_extra->model;

	entity = std::make_shared<Entity>(model_resource.name);

	LOG_ASSERT(model->mesh_data_list.size() == 1
		&& "We are assuming that skybox models only have one mesh");
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

	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.vertices.size() 
		* sizeof(MeshVertex),
		mesh_data.vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, (3 * 3 + 2) * 4, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_coordinates);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.vertices.size()
		* sizeof(MeshVertex), mesh_data.vertices.data(), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	const void* offset = (void*) ((3 * 4) * 4);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, (3 * 4) * 4, offset);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data.indices.size(),
		mesh_data.indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

SkyBox::~SkyBox()
{
	glDeleteBuffers(SKYBOX_VBO_COUNT, vbo_list);
	glDeleteVertexArrays(1, &vao);
}