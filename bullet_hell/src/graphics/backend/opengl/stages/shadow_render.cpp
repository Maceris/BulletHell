#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/shadow_render.h"

#include "debugging/logger.h"
#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/graph/shadow_buffer.h"

#include "glad.h"

ShadowRender::ShadowRender(StageResource<RenderBuffers>* render_buffers,
	StageResource<CascadeShadows>* cascade_shadows,
	StageResource<Framebuffer>* depth_map,
	StageResource<CommandBuffers>* command_buffers)
	: render_buffers{ render_buffers }
	, cascade_shadows{ cascade_shadows }
	, depth_map{ depth_map }
	, command_buffers{ command_buffers }
{
	std::vector<Shader::Module> shader_modules;
	shader_modules.emplace_back("shaders/shadow.vert",
		Shader::Type::VERTEX);

	shader = ALLOC Shader(shader_modules);
	
	shader->uniforms.create_uniform("projection_view_matrix");
}

void ShadowRender::render(Scene& scene)
{
    CascadeShadowSlice::updateCascadeShadows(**cascade_shadows, scene);

    glBindFramebuffer(GL_FRAMEBUFFER, (*depth_map)->handle);
    glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

    shader->bind();

    for (unsigned int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, (*depth_map)->textures[i], 0);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    //NOTE(ches) static meshes
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DRAW_ELEMENT_BINDING,
        (*command_buffers)->static_draw_element_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MODEL_MATRICES_BINDING,
        (*command_buffers)->static_model_matrices_buffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
        (*command_buffers)->static_command_buffer);
    glBindVertexArray((*render_buffers)->static_vao);
    for (unsigned int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, (*depth_map)->textures[i], 0);

        shader->uniforms.set_uniform("projection_view_matrix",
            (**cascade_shadows)[i].projection_view_matrix);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
            (*command_buffers)->static_draw_count, 0);
    }

    //NOTE(ches) animated meshes
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DRAW_ELEMENT_BINDING,
        (*command_buffers)->animated_draw_element_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MODEL_MATRICES_BINDING,
        (*command_buffers)->animated_model_matrices_buffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
        (*command_buffers)->animated_command_buffer);
    glBindVertexArray((*render_buffers)->animated_vao);
    for (unsigned int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D, (*depth_map)->textures[i], 0);

        shader->uniforms.set_uniform("projection_view_matrix",
            (**cascade_shadows)[i].projection_view_matrix);
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
            (*command_buffers)->animated_draw_count, 0);
    }

    glBindVertexArray(0);
    shader->unbind();
}

#endif
