#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/render/sky_box_render.h"

#include "debugging/logger.h"
#include "graphics/frontend/texture.h"
#include "graphics/frontend/texture_loader.h"
#include "graphics/graph/material.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/graph/texture_resource.h"
#include "graphics/scene/scene.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

#include "glad.h"

SkyBoxRender::SkyBoxRender()
{
    Resource frag("shaders/skybox.frag");
    std::shared_ptr<ResourceHandle> frag_handle =
        g_game_logic->resource_cache->get_handle(&frag);
    LOG_ASSERT(frag_handle && "Cannot find skybox fragment shader");

    Resource vert("shaders/skybox.vert");
    std::shared_ptr<ResourceHandle> vert_handle =
        g_game_logic->resource_cache->get_handle(&vert);
    LOG_ASSERT(vert_handle && "Cannot find skybox vertex shader");

    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(vert_handle->get_buffer(),
        vert_handle->get_size(), GL_VERTEX_SHADER);
    shader_modules.emplace_back(frag_handle->get_buffer(),
        frag_handle->get_size(), GL_FRAGMENT_SHADER);
    shader_program = std::make_unique<ShaderProgram>(shader_modules);

    uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);
    create_uniforms();
}

void SkyBoxRender::render(const Scene& scene)
{
    const SkyBox& sky_box = scene.sky_box;

    shader_program->bind();

    uniforms_map->set_uniform("projection_matrix",
        scene.projection.projection_matrix);
    glm::mat4 view_matrix(scene.camera.view_matrix);
    //NOTE(ches) directly set transform to 0
    view_matrix[3][0] = 0;
    view_matrix[3][1] = 0;
    view_matrix[3][2] = 0;
    uniforms_map->set_uniform("view_matrix", view_matrix);
    uniforms_map->set_uniform("texture_sampler", 0);

    const auto& material = sky_box.model->mesh_data_list[0].material;
    uniforms_map->set_uniform("diffuse", material->diffuse_color);

    bool has_texture = false;
    if (material->texture_name != "")
    {
        glActiveTexture(GL_TEXTURE0);
        auto texture = TextureLoader::load(material->texture_name);
        glBindTexture(GL_TEXTURE_2D, texture.handle);
        has_texture = true;
    }

    uniforms_map->set_uniform("has_texture", has_texture ? 1 : 0);

    glBindVertexArray(sky_box.vao);

    uniforms_map->set_uniform("model_matrix", sky_box.entity->model_matrix);
    glDrawElements(GL_TRIANGLES, sky_box.index_count, GL_UNSIGNED_INT,
        nullptr);

    glBindVertexArray(0);
    shader_program->unbind();
}

void SkyBoxRender::create_uniforms()
{
    uniforms_map->create_uniform("projection_matrix");
    uniforms_map->create_uniform("view_matrix");
    uniforms_map->create_uniform("model_matrix");
    uniforms_map->create_uniform("diffuse");
    uniforms_map->create_uniform("texture_sampler");
    uniforms_map->create_uniform("has_texture");
}
#endif
