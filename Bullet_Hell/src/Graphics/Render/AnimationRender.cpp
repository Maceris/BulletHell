#include "Globals.h"
#include "AnimationRender.h"

#include <cmath>
#include <vector>

#pragma region Shader code
const char compute_shader_source[] = R"glsl(
#version 460

layout (std430, binding=0) readonly buffer source_buffer {
    float data[];
} source_vector;

layout (std430, binding=1) readonly buffer weights_buffer {
    float data[];
} weights_vector;

layout (std430, binding=2) readonly buffer bones_buffer {
    mat4 data[];
} bones_matrices;

layout (std430, binding=3) buffer destination_buffer {
    float data[];
} destination_vector;

struct DrawParameters
{
    int source_offset;
    int source_size;
    int weights_offset;
    int bones_matrices_offset;
    int destination_offset;
};
uniform DrawParameters draw_parameters;

layout (local_size_x=1, local_size_y=1, local_size_z=1) in;

void main()
{
    int base_index = int(gl_GlobalInvocationID.x) * 14;
    uint weights_buffer_base_index  = draw_parameters.weights_offset + int(gl_GlobalInvocationID.x) * 8;
    uint source_buffer_base_index = draw_parameters.source_offset + base_index;
    uint destination_buffer_base_index = draw_parameters.destination_offset + base_index;
    if (base_index >= draw_parameters.source_size) {
        return;
    }

    vec4 weights = vec4(
        weights_vector.data[weights_buffer_base_index],
        weights_vector.data[weights_buffer_base_index + 1],
        weights_vector.data[weights_buffer_base_index + 2],
        weights_vector.data[weights_buffer_base_index + 3]);
    ivec4 bones_indices = ivec4(
        weights_vector.data[weights_buffer_base_index + 4],
        weights_vector.data[weights_buffer_base_index + 5],
        weights_vector.data[weights_buffer_base_index + 6],
        weights_vector.data[weights_buffer_base_index + 7]);

    vec4 position = vec4(
        source_vector.data[source_buffer_base_index],
        source_vector.data[source_buffer_base_index + 1],
        source_vector.data[source_buffer_base_index + 2],
        1);
    position = 
      weights.x * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.x] * position
    + weights.y * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.y] * position
    + weights.z * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.z] * position
    + weights.w * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.w] * position;

    destination_vector.data[destination_buffer_base_index] = position.x / position.w;
    destination_vector.data[destination_buffer_base_index + 1] = position.y / position.w;
    destination_vector.data[destination_buffer_base_index + 2] = position.z / position.w;

    source_buffer_base_index += 3;
    destination_buffer_base_index += 3;

    vec4 normal = vec4
        source_vector.data[source_buffer_base_index],
        source_vector.data[source_buffer_base_index + 1],
        source_vector.data[source_buffer_base_index + 2],
        0);
    normal =
      weights.x * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.x] * normal
    + weights.y * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.y] * normal
    + weights.z * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.z] * normal
    + weights.w * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.w] * normal;

    destination_vector.data[destination_buffer_base_index] = normal.x;
    destination_vector.data[destination_buffer_base_index + 1] = normal.y;
    destination_vector.data[destination_buffer_base_index + 2] = normal.z;

    source_buffer_base_index += 3;
    destination_buffer_base_index += 3;

    vec4 tangent = vec4
        source_vector.data[source_buffer_base_index],
        source_vector.data[source_buffer_base_index + 1],
        source_vector.data[source_buffer_base_index + 2],
        0);
    tangent =
      weights.x * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.x] * tangent
    + weights.y * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.y] * tangent
    + weights.z * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.z] * tangent
    + weights.w * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.w] * tangent;

    destination_vector.data[destination_buffer_base_index] = tangent.x;
    destination_vector.data[destination_buffer_base_index + 1] = tangent.y;
    destination_vector.data[destination_buffer_base_index + 2] = tangent.z;

    source_buffer_base_index += 3;
    destination_buffer_base_index += 3;

    vec4 bitangent = vec4(
        source_vector.data[source_buffer_base_index],
        source_vector.data[source_buffer_base_index + 1],
        source_vector.data[source_buffer_base_index + 2],
        0);
    bitangent =
      weights.x * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.x] * bitangent
    + weights.y * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.y] * bitangent
    + weights.z * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.z] * bitangent
    + weights.w * bones_matrices.data[draw_parameters.bones_matrices_offset + bones_indices.w] * bitangent;

    destination_vector.data[destination_buffer_base_index] = bitangent.x;
    destination_vector.data[destination_buffer_base_index + 1] = bitangent.y;
    destination_vector.data[destination_buffer_base_index + 2] = bitangent.z;

    source_buffer_base_index += 3;
    destination_buffer_base_index += 3;

    vec2 texture_coordinates = vec2
        source_vector.data[source_buffer_base_index],
        source_vector.data[source_buffer_base_index + 1]);
    destination_vector.data[destination_buffer_base_index] = texture_coordinates.x;
    destination_vector.data[destination_buffer_base_index + 1] = texture_coordinates.y;
}
)glsl";
#pragma endregion


AnimationRender::AnimationRender()
{
    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(compute_shader_source,
        sizeof(compute_shader_source), GL_COMPUTE_SHADER);

    shader_program = std::make_unique<ShaderProgram>(shader_modules);
    uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);

    uniforms_map->create_uniform("draw_parameters.source_offset");
    uniforms_map->create_uniform("draw_parameters.source_size");
    uniforms_map->create_uniform("draw_parameters.weights_offset");
    uniforms_map->create_uniform("draw_parameters.bones_matrices_offset");
    uniforms_map->create_uniform("draw_parameters.destination_offset");
}

void AnimationRender::render(const Scene& scene,
    const RenderBuffers& render_buffer)
{
    shader_program->bind();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 
        render_buffer.binding_poses_vbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
        render_buffer.bones_indices_weights_vbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
        render_buffer.bones_matrices_vbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3,
        render_buffer.dest_animation_vbo);

    int destination_offset = 0;
    for (auto entry : scene.model_map)
    {
        std::shared_ptr<Model> model = entry.second;
        if (!model->is_animated())
        {
            continue;
        }
        for (MeshDrawData mesh_draw_data : model->mesh_draw_data_list)
        {
            AnimMeshDrawData anim_mesh_draw_data = 
                mesh_draw_data.animated_mesh_draw_data;
            std::shared_ptr<Entity> entity = anim_mesh_draw_data.entity;
            AnimatedFrame frame = entity->animation_data.get_current_frame();
            int group_size = std::ceilf(
                (float) mesh_draw_data.size_in_bytes / (14 * 4));
            
            uniforms_map->set_uniform("draw_parameters.source_offset", 
                anim_mesh_draw_data.binding_pose_offset);
            uniforms_map->set_uniform("draw_parameters.source_size",
                mesh_draw_data.size_in_bytes / 4);
            uniforms_map->set_uniform("draw_parameters.weights_offset",
                anim_mesh_draw_data.weights_offset);
            uniforms_map->set_uniform("draw_parameters.bones_matrices_offset",
               frame.offset);
            uniforms_map->set_uniform("draw_parameters.destination_offset",
                destination_offset);

            glDispatchCompute(group_size, 1, 1);
            destination_offset += mesh_draw_data.size_in_bytes / 4;
        }
    }
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    shader_program->unbind();
}