#include "graphics/render/animation_render.h"

#include <cmath>
#include <vector>

#include "debugging/logger.h"
#include "graphics/graph/animation.h"
#include "graphics/graph/mesh_draw_data.h"
#include "graphics/render/render_buffers.h"
#include "graphics/scene/scene.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

#include "glad.h"

AnimationRender::AnimationRender()
{
    Resource compute("shaders/animation.compute");
    std::shared_ptr<ResourceHandle> compute_handle =
        g_game_logic->resource_cache->get_handle(&compute);
    LOG_ASSERT(compute_handle && "Cannot find animation compute shader");

    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(compute_handle->get_buffer(),
        compute_handle->get_size(), GL_COMPUTE_SHADER);

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