#include "ShadowRender.h"

#pragma region Shader code
const char vertex_shader_source[] = R"glsl(
#version 460

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 tangent;
layout (location=3) in vec3 bitangent;
layout (location=4) in vec2 texture_coordinates;

struct DrawElement
{
    int model_matrix_index;
    int material_index;
};

uniform mat4 projection_view_matrix;

layout (std430, binding=0) buffer DrawElements {
    DrawElement draw_elements[];
};

layout (std430, binding=1) buffer Matrices {
	mat4 model_matrices[];
};

void main()
{
    vec4 initial_position = vec4(position, 1.0);
    uint index = gl_BaseInstance + gl_InstanceID;
	DrawElement draw_element = draw_elements[index];
    mat4 model_matrix = model_matrices[draw_element.model_matrix_index];
    gl_Position = projection_view_matrix * model_matrix * initial_position;
}
)glsl";
#pragma endregion

ShadowRender::ShadowRender()
    : shadow_buffer{}
    , cascade_shadows{}
{
    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(vertex_shader_source,
        sizeof(vertex_shader_source), GL_VERTEX_SHADER);
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