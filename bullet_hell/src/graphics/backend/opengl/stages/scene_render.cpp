#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/scene_render.h"

#include <algorithm>

#include "debugging/logger.h"
#include "main/game_logic.h"
#include "graphics/render_constants.h"
#include "graphics/backend/base/pipeline_manager.h"
#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/texture.h"
#include "graphics/frontend/texture_loader.h"
#include "graphics/graph/texture_resource.h"
#include "graphics/graph/gbuffer.h"
#include "graphics/graph/material.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/scene/scene.h"
#include "main/game_logic.h"

#include "glad.h"

SceneRender::SceneRender(StageResource<RenderBuffers>* render_buffers,
	StageResource<Framebuffer>* gbuffer,
	StageResource<CommandBuffers>* command_buffers,
	StageResource<Texture>* default_texture)
	: render_buffers{ render_buffers }
	, gbuffer{ gbuffer }
	, command_buffers{ command_buffers }
	, default_texture{ default_texture }
{
	std::vector<Shader::Module> shader_modules;
	shader_modules.emplace_back("shaders/scene.frag",
		Shader::Type::FRAGMENT);
	shader_modules.emplace_back("shaders/scene.vert",
		Shader::Type::VERTEX);

	shader = ALLOC Shader(shader_modules);
	create_uniforms();
}


void SceneRender::create_uniforms()
{
    shader->uniforms.create_uniform("projection_matrix");
    shader->uniforms.create_uniform("view_matrix");

    for (int i = 0; i < MAX_TEXTURES; ++i)
    {
        shader->uniforms.create_uniform(std::format("texture_sampler[{}]", i));
    }

    for (int i = 0; i < MAX_MATERIALS; ++i)
    {
        shader->uniforms.create_uniform(std::format("materials[{}].diffuse", i));
        shader->uniforms.create_uniform(std::format("materials[{}].specular", i));
        shader->uniforms.create_uniform(std::format("materials[{}].reflectance", i));
        shader->uniforms.create_uniform(std::format("materials[{}].normal_map_index", i));
        shader->uniforms.create_uniform(std::format("materials[{}].texture_index", i));
    }
}

template<typename T>
int constexpr find(const std::vector<T>& list, const T& value)
{
    auto result = std::find(list.cbegin(), list.cend(), value);

    if (result != list.cend())
    {
        return result - list.cbegin();
    }
    return 0;
}

void SceneRender::render(const Scene& scene)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, (*gbuffer)->handle);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, (*gbuffer)->width, (*gbuffer)->height);
    glDisable(GL_BLEND);
    shader->bind();

    shader->uniforms.set_uniform("projection_matrix",
        scene.projection.projection_matrix);
    shader->uniforms.set_uniform("view_matrix",
        scene.camera.view_matrix);

    const bool for_animated_models = true;
    const bool for_static_models = false;

    setup_materials_uniform(scene, for_static_models);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DRAW_ELEMENT_BINDING,
        (*command_buffers)->static_draw_element_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MODEL_MATRICES_BINDING,
        (*command_buffers)->static_model_matrices_buffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
        (*command_buffers)->static_command_buffer);
    glBindVertexArray((*render_buffers)->static_vao);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
        (*command_buffers)->static_draw_count, 0);

    setup_materials_uniform(scene, for_animated_models);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DRAW_ELEMENT_BINDING,
        (*command_buffers)->animated_draw_element_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MODEL_MATRICES_BINDING,
        (*command_buffers)->animated_model_matrices_buffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
        (*command_buffers)->animated_command_buffer);
    glBindVertexArray((*render_buffers)->animated_vao);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
        (*command_buffers)->animated_draw_count, 0);

    glBindVertexArray(0);
    glEnable(GL_BLEND);
    shader->unbind();
}

void SceneRender::setup_materials_uniform(const Scene& scene,
    const bool animated)
{
    const int first_index = 1;

    std::vector<std::string> texture_bindings;

    const auto& model_list = animated
        ? scene.get_animated_model_list()
        : scene.get_static_model_list();

    for (const auto& model : model_list)
    {
        for (const auto& mesh_data : model->mesh_data_list)
        {
            const auto& material = mesh_data.material;
  
            shader->uniforms.set_uniform(
                std::format("materials[{}].diffuse", material->material_id),
                material->diffuse_color);
            shader->uniforms.set_uniform(
                std::format("materials[{}].specular", material->material_id), 
                material->specular_color);
            shader->uniforms.set_uniform(
                std::format("materials[{}].reflectance", material->material_id), 
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
            shader->uniforms.set_uniform(
                std::format("materials[{}].texture_index", material->material_id), 
                index);

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
            shader->uniforms.set_uniform(
                std::format("materials[{}].normal_map_index", material->material_id), 
                index);
        }
    }

    LOG_ASSERT(texture_bindings.size() + 1 <= MAX_TEXTURES
        && "We have more textures than we can bind in one draw call");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, PipelineManager::default_texture->handle);

    int start_texture = 1;
    for (int i = 0; i < texture_bindings.size(); ++i)
    {
        const std::string& texture_name = texture_bindings[i];
        auto texture = TextureLoader::load(texture_name);
        const int current_texture = start_texture + i;

        glActiveTexture(GL_TEXTURE0 + current_texture);
        glBindTexture(GL_TEXTURE_2D, texture.handle);
        
        shader->uniforms.set_uniform(
            std::format("texture_sampler[{}]", current_texture),
            current_texture);
    }
}

#endif
