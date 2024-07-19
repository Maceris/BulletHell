#include "graphics/render/scene_render.h"

#include <unordered_set>

#include "debugging/logger.h"
#include "main/game_logic.h"
#include "graphics/graph/texture_resource.h"
#include "graphics/graph/gbuffer.h"
#include "graphics/graph/material.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/graph/texture.h"
#include "graphics/render/command_buffers.h"
#include "graphics/render/render_buffers.h"
#include "graphics/render/render_constants.h"
#include "graphics/scene/scene.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

#include "glad.h"

SceneRender::SceneRender()
{
    Resource frag("shaders/scene.frag");
    std::shared_ptr<ResourceHandle> frag_handle =
        g_game_logic->resource_cache->get_handle(&frag);
    LOG_ASSERT(frag_handle && "Cannot find scene fragment shader");

    Resource vert("shaders/scene.vert");
    std::shared_ptr<ResourceHandle> vert_handle =
        g_game_logic->resource_cache->get_handle(&vert);
    LOG_ASSERT(vert_handle && "Cannot find scene vertex shader");

    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(vert_handle->get_buffer(),
        vert_handle->get_size(), GL_VERTEX_SHADER);
    shader_modules.emplace_back(frag_handle->get_buffer(),
        frag_handle->get_size(), GL_FRAGMENT_SHADER);
    shader_program = std::make_unique<ShaderProgram>(shader_modules);

    uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);
    create_uniforms();
}

void SceneRender::render(const Scene& scene, const RenderBuffers& render_buffers,
    const GBuffer& gBuffer, const CommandBuffers& command_buffers)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer.gBuffer_ID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, gBuffer.width, gBuffer.height);
    glDisable(GL_BLEND);
    shader_program->bind();

    uniforms_map->set_uniform("projection_matrix", 
        scene.projection.projection_matrix);
    uniforms_map->set_uniform("view_matrix",
        scene.camera.view_matrix);

    const bool for_animated_models = true;
    const bool for_static_models = false;
    
    setup_materials_uniform(scene, for_static_models);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DRAW_ELEMENT_BINDING,
        command_buffers.static_draw_element_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MODEL_MATRICES_BINDING,
        command_buffers.static_model_matrices_buffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
        command_buffers.static_command_buffer);
    glBindVertexArray(render_buffers.static_vao);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
        command_buffers.static_draw_count, 0);

    setup_materials_uniform(scene, for_animated_models);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DRAW_ELEMENT_BINDING,
        command_buffers.animated_draw_element_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MODEL_MATRICES_BINDING,
        command_buffers.animated_model_matrices_buffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
        command_buffers.animated_command_buffer);
    glBindVertexArray(render_buffers.animated_vao);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
        command_buffers.animated_draw_count, 0);

    glBindVertexArray(0);
    glEnable(GL_BLEND);
    shader_program->unbind();
}

void SceneRender::create_uniforms()
{
    uniforms_map->create_uniform("projection_matrix");
    uniforms_map->create_uniform("view_matrix");

    for (int i = 0; i < MAX_TEXTURES; ++i)
    {
        uniforms_map->create_uniform("texture_sampler[" + std::to_string(i) 
            + "]");
    }

    for (int i = 0; i < MAX_MATERIALS; ++i)
    {
        const std::string prefix = "materials[" + std::to_string(i) + "].";
        uniforms_map->create_uniform(prefix + "diffuse");
        uniforms_map->create_uniform(prefix + "specular");
        uniforms_map->create_uniform(prefix + "reflectance");
        uniforms_map->create_uniform(prefix + "normal_map_index");
        uniforms_map->create_uniform(prefix + "texture_index");
    }
}

template<typename T>
int constexpr find(const std::vector<T>& list, const T& value)
{
    int i = 0;
    for (i = 0; i < list.size(); ++i)
    {
        if (list[i] == value)
        {
            return i;
        }
    }
    return i;
}

void SceneRender::setup_materials_uniform(const Scene& scene, 
    const bool animated)
{
    const int first_index = 1;

    std::vector<std::string> texture_bindings;

    const auto& model_list = animated 
        ? scene.get_animated_model_list() 
        : scene.get_static_model_list();

    for (const auto& model: model_list)
    {
        for (const auto& mesh_data : model->mesh_data_list)
        {
            const auto& material = mesh_data.material;

            const std::string prefix = "materials[" 
                + std::to_string(material->material_id) + "].";

            uniforms_map->set_uniform(prefix + "diffuse",
                material->diffuse_color);
            uniforms_map->set_uniform(prefix + "specular",
                material->specular_color);
            uniforms_map->set_uniform(prefix + "reflectance",
                material->reflectance);

            // Default texture is 0
            int index = 0;
            if (material->texture_name != "")
            {
                int position_in_vector = 
                    find(texture_bindings, material->texture_name);
                if (position_in_vector == texture_bindings.size()) {
                    texture_bindings.push_back(material->texture_name);
                }
                index = position_in_vector + first_index;
            }
            uniforms_map->set_uniform(prefix + "texture_index", index);

            index = 0;
            if (material->normal_map_name != "")
            {
                int position_in_vector = 
                    find(texture_bindings, material->normal_map_name);
                if (position_in_vector == texture_bindings.size()) {
                    texture_bindings.push_back(material->normal_map_name);
                }
                index = position_in_vector + first_index;
            }
            uniforms_map->set_uniform(prefix + "normal_map_index", index);
        }
    }

    LOG_ASSERT(texture_bindings.size() + 1 <= MAX_TEXTURES
        && "We have more textures than we can bind in one draw call");

    glActiveTexture(GL_TEXTURE0);
    Texture::default_texture->bind();

    int start_texture = 1;
    for (int i = 0; i < texture_bindings.size(); ++i)
    {
        const std::string& texture_name = texture_bindings[i];
        auto texture = load_texture(texture_name);
        glActiveTexture(GL_TEXTURE0 + start_texture + i);
        texture->bind();
        uniforms_map->set_uniform("texture_sampler["
            + std::to_string(start_texture + i) + "]", start_texture + i);
    }
}
