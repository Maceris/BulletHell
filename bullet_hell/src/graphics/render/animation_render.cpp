#include "graphics/render/animation_render.h"

#include <cmath>
#include <vector>

#include "debugging/logger.h"
#include "graphics/graph/animation.h"
#include "graphics/graph/mesh_draw_data.h"
#include "graphics/render/render_buffers.h"
#include "graphics/scene/scene.h"

#include "glad.h"

#pragma region Shader code
const char compute_shader_source[] = R"glsl(
#version 460

struct DrawParameters
{
    int source_offset;
    int source_size;
    int weights_offset;
    int bones_matrices_offset;
    int destination_offset;
};

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

layout (std430, binding=4) readonly buffer draw_parameters_buffer {
    DrawParameters data[];
} draw_parameters;

layout (local_size_x=1, local_size_y=1, local_size_z=1) in;

uniform int base_draw_parameter;

void main()
{
    int base_index = int(gl_GlobalInvocationID.x) * 14;
    int draw_index = base_draw_parameter + int(gl_GlobalInvocationID.y);
    if (base_index >= draw_parameters.data[draw_index].source_size) {
        return;
    }
    
    uint destination_buffer_base_index = draw_parameters.data[draw_index].destination_offset + base_index;
    
    uint weights_buffer_base_index = draw_parameters.data[draw_index].weights_offset + int(gl_GlobalInvocationID.x) * 8;
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

    mat4 bone_weights_x = weights.x * bones_matrices.data[draw_parameters.data[draw_index].bones_matrices_offset + bones_indices.x];
    mat4 bone_weights_y = weights.y * bones_matrices.data[draw_parameters.data[draw_index].bones_matrices_offset + bones_indices.y];
    mat4 bone_weights_z = weights.z * bones_matrices.data[draw_parameters.data[draw_index].bones_matrices_offset + bones_indices.z];
    mat4 bone_weights_w = weights.w * bones_matrices.data[draw_parameters.data[draw_index].bones_matrices_offset + bones_indices.w];

    uint source_buffer_base_index = draw_parameters.data[draw_index].source_offset + base_index;
    vec4 position = vec4(
        source_vector.data[source_buffer_base_index],
        source_vector.data[source_buffer_base_index + 1],
        source_vector.data[source_buffer_base_index + 2],
        1);
    position = 
          bone_weights_x * position
        + bone_weights_y * position
        + bone_weights_z * position
        + bone_weights_w * position;

    destination_vector.data[destination_buffer_base_index] = position.x / position.w;
    destination_vector.data[destination_buffer_base_index + 1] = position.y / position.w;
    destination_vector.data[destination_buffer_base_index + 2] = position.z / position.w;

    source_buffer_base_index += 3;
    destination_buffer_base_index += 3;

    vec4 normal = vec4(
        source_vector.data[source_buffer_base_index],
        source_vector.data[source_buffer_base_index + 1],
        source_vector.data[source_buffer_base_index + 2],
        0);
    normal = 
          bone_weights_x * normal
        + bone_weights_y * normal
        + bone_weights_z * normal
        + bone_weights_w * normal;

    destination_vector.data[destination_buffer_base_index] = normal.x;
    destination_vector.data[destination_buffer_base_index + 1] = normal.y;
    destination_vector.data[destination_buffer_base_index + 2] = normal.z;

    source_buffer_base_index += 3;
    destination_buffer_base_index += 3;

    vec4 tangent = vec4(
        source_vector.data[source_buffer_base_index],
        source_vector.data[source_buffer_base_index + 1],
        source_vector.data[source_buffer_base_index + 2],
        0);
    tangent = 
          bone_weights_x * tangent
        + bone_weights_y * tangent
        + bone_weights_z * tangent
        + bone_weights_w * tangent;

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
          bone_weights_x * bitangent
        + bone_weights_y * bitangent
        + bone_weights_z * bitangent
        + bone_weights_w * bitangent;

    destination_vector.data[destination_buffer_base_index] = bitangent.x;
    destination_vector.data[destination_buffer_base_index + 1] = bitangent.y;
    destination_vector.data[destination_buffer_base_index + 2] = bitangent.z;

    source_buffer_base_index += 3;
    destination_buffer_base_index += 3;

    vec2 texture_coordinates = vec2(
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
    LOG_ASSERT(compute_shader_source && "Shader source missing!");
    shader_modules.emplace_back(compute_shader_source,
        sizeof(compute_shader_source), GL_COMPUTE_SHADER);

    shader_program = std::make_unique<ShaderProgram>(shader_modules);
    uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);
    uniforms_map->create_uniform("base_draw_parameter");
}

struct RenderInfo
{
    int model_vertex_count;
    int entity_count;
};

void AnimationRender::render(const Scene& scene,
    const RenderBuffers& render_buffer)
{
    const auto& model_list = scene.get_animated_model_list();

    int destination_offset = 0;
    std::map<std::string, RenderInfo> render_info;
    int parameter_count = 0;
    std::vector<int> parameter_list;
    for (const auto& model : model_list)
    {
        if (model->entity_list.empty())
        {
            continue;
        }

        int model_vertex_count = 0;
        const int entity_count = model->entity_list.size();

        for (const auto& mesh_data : model->mesh_data_list)
        {
            model_vertex_count += mesh_data.vertices.size();
        }

        for (const auto& mesh_draw_data : model->mesh_draw_data_list)
        {
            const AnimMeshDrawData& anim_mesh_draw_data =
                mesh_draw_data.animated_mesh_draw_data;
            std::shared_ptr<Entity> entity = anim_mesh_draw_data.entity;
            const AnimatedFrame& frame = 
                entity->animation_data.get_current_frame();
            
            parameter_list.push_back(anim_mesh_draw_data.binding_pose_offset);
            parameter_list.push_back(mesh_draw_data.size_in_bytes / 4);
            parameter_list.push_back(anim_mesh_draw_data.weights_offset);
            parameter_list.push_back(static_cast<int>(frame.offset));
            parameter_list.push_back(destination_offset);
            ++parameter_count;
            destination_offset += mesh_draw_data.size_in_bytes / 4;
        }

        render_info.emplace(
            model->id, 
            RenderInfo{ model_vertex_count, entity_count }
        );
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,
        render_buffer.animation_draw_parameters_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        parameter_list.size() * sizeof(int), parameter_list.data(),
        GL_STATIC_DRAW);

    shader_program->bind();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
        render_buffer.binding_poses_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
        render_buffer.bones_indices_weights_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
        render_buffer.bones_matrices_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3,
        render_buffer.dest_animation_vbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4,
        render_buffer.animation_draw_parameters_ssbo);

    int mesh_count = 0;
    int base_draw_parameter = 0;
    for (const auto& model : model_list)
    {
        if (model->entity_list.empty())
        {
            continue;
        }
        const RenderInfo& info = render_info.find(model->id)->second;
        
        mesh_count = model->mesh_data_list.size();
        uniforms_map->set_uniform("base_draw_parameter", base_draw_parameter);
        glDispatchCompute(info.model_vertex_count, info.entity_count * mesh_count, 1);
        base_draw_parameter += info.entity_count * mesh_count;
    }

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    shader_program->unbind();
}