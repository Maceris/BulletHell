#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/skybox_render.h"

#include "debugging/logger.h"
#include "debugging/timer.h"
#include "graphics/frontend/texture.h"
#include "graphics/frontend/texture_loader.h"
#include "graphics/graph/material.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/graph/texture_resource.h"
#include "graphics/scene/scene.h"

#include "glad.h"

struct SkyboxRender::Data
{
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
};

SkyboxRender::SkyboxRender(StageResource<SkyBox>* skybox)
	: skybox{ skybox }
	, data{ std::make_unique<SkyboxRender::Data>() }
{
	std::vector<Shader::Module> shader_modules;
	shader_modules.emplace_back("shaders/skybox.frag",
		Shader::Type::FRAGMENT);
	shader_modules.emplace_back("shaders/skybox.vert",
		Shader::Type::VERTEX);

	shader = ALLOC Shader(shader_modules);

	shader->uniforms.create_uniform("projection_matrix");
	shader->uniforms.create_uniform("view_matrix");
	shader->uniforms.create_uniform("model_matrix");
	shader->uniforms.create_uniform("diffuse");
	shader->uniforms.create_uniform("texture_sampler");
	shader->uniforms.create_uniform("has_texture");

    MeshData& mesh_data = (*skybox)->model->mesh_data_list[0];
    data->index_count = static_cast<int>(mesh_data.indices.size());
    
	glGenVertexArrays(1, &(data->vao));
	glBindVertexArray(data->vao);

	glGenBuffers(SKYBOX_VBO_COUNT, data->vbo_list);

	const GLuint vbo_positions = data->vbo_list[0];
	const GLuint vbo_texture_coordinates = data->vbo_list[1];
	const GLuint vbo_indices = data->vbo_list[2];

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

	safe_delete_array(positions);
	safe_delete_array(texture_coordinates);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

SkyboxRender::~SkyboxRender()
{
	glDeleteBuffers(SKYBOX_VBO_COUNT, data->vbo_list);
	glDeleteVertexArrays(1, &(data->vao));
}

void SkyboxRender::render(Scene& scene)
{
    TIME_START("Skybox Render");
    const SkyBox& sky_box = scene.sky_box;

    shader->bind();

    shader->uniforms.set_uniform("projection_matrix",
        scene.projection.projection_matrix);
    glm::mat4 view_matrix(scene.camera.view_matrix);
    //NOTE(ches) directly set transform to 0
    view_matrix[3][0] = 0;
    view_matrix[3][1] = 0;
    view_matrix[3][2] = 0;
    shader->uniforms.set_uniform("view_matrix", view_matrix);
    shader->uniforms.set_uniform("texture_sampler", 0);

    const auto& material = (*skybox)->model->mesh_data_list[0].material;
    shader->uniforms.set_uniform("diffuse", material->diffuse_color);

    bool has_texture = false;
    if (material->texture_name != "")
    {
        glActiveTexture(GL_TEXTURE0);
        auto texture = TextureLoader::load(material->texture_name);
        glBindTexture(GL_TEXTURE_2D, texture.handle);
        has_texture = true;
    }

    shader->uniforms.set_uniform("has_texture", has_texture ? 1 : 0);

	glBindVertexArray(data->vao);

    shader->uniforms.set_uniform("model_matrix", (*skybox)->entity->model_matrix);
    glDrawElements(GL_TRIANGLES, data->index_count, GL_UNSIGNED_INT,
        nullptr);

    glBindVertexArray(0);
    shader->unbind();
    TIME_END("Skybox Render");
}

#endif
