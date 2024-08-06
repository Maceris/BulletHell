#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/skybox_render.h"

#include "debugging/logger.h"
#include "graphics/frontend/texture.h"
#include "graphics/frontend/texture_loader.h"
#include "graphics/graph/material.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/graph/texture_resource.h"
#include "graphics/scene/scene.h"

#include "glad.h"

SkyboxRender::SkyboxRender(StageResource<SkyBox>* skybox)
	: skybox{ skybox }
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
}

void SkyboxRender::render(Scene& scene)
{
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

    glBindVertexArray((*skybox)->vao);

    shader->uniforms.set_uniform("model_matrix", (*skybox)->entity->model_matrix);
    glDrawElements(GL_TRIANGLES, (*skybox)->index_count, GL_UNSIGNED_INT,
        nullptr);

    glBindVertexArray(0);
    shader->unbind();
}

#endif
