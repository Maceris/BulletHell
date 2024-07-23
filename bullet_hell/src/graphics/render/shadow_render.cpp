#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/render/shadow_render.h"

#include "debugging/logger.h"
#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/graph/shadow_buffer.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

#include "glad.h"

ShadowRender::ShadowRender()
    : shadow_buffer{}
    , cascade_shadows{}
{
    Resource vert("shaders/shadow.vert");
    std::shared_ptr<ResourceHandle> vert_handle =
        g_game_logic->resource_cache->get_handle(&vert);
    LOG_ASSERT(vert_handle && "Cannot find shadow vertex shader");

    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(vert_handle->get_buffer(),
        vert_handle->get_size(), GL_VERTEX_SHADER);
    shader_program = std::make_unique<ShaderProgram>(shader_modules);

    for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        cascade_shadows.emplace_back();
    }

    uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);
    uniforms_map->create_uniform("projection_view_matrix");
}

void ShadowRender::render(const Scene& scene,
    const RenderBuffers& render_buffers,
    const CommandBuffers& command_buffers)
{
    CascadeShadowSlice::updateCascadeShadows(cascade_shadows, scene);

    glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer.depth_map_fbo);
    glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

    shader_program->bind();

    for (unsigned int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, shadow_buffer.depth_map[i], 0);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    //NOTE(ches) static meshes
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DRAW_ELEMENT_BINDING,
        command_buffers.static_draw_element_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MODEL_MATRICES_BINDING,
        command_buffers.static_model_matrices_buffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
        command_buffers.static_command_buffer);
    glBindVertexArray(render_buffers.static_vao);
    for (unsigned int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, shadow_buffer.depth_map[i], 0);

        uniforms_map->set_uniform("projection_view_matrix",
            cascade_shadows[i].projection_view_matrix);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
            command_buffers.static_draw_count, 0);
    }

    //NOTE(ches) animated meshes
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DRAW_ELEMENT_BINDING,
        command_buffers.animated_draw_element_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MODEL_MATRICES_BINDING,
        command_buffers.animated_model_matrices_buffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
        command_buffers.animated_command_buffer);
    glBindVertexArray(render_buffers.animated_vao);
    for (unsigned int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, shadow_buffer.depth_map[i], 0);

        uniforms_map->set_uniform("projection_view_matrix",
            cascade_shadows[i].projection_view_matrix);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
            command_buffers.animated_draw_count, 0);
    }

    glBindVertexArray(0);
    shader_program->unbind();
}
#endif
