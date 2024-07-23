#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include <cmath>
#include <vector>

#include "debugging/logger.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/backend/opengl/stages/animation_render.h"
#include "graphics/graph/animation.h"
#include "graphics/graph/mesh_draw_data.h"
#include "graphics/scene/scene.h"
#include "main/game_logic.h"

#include "glad.h"

struct RenderInfo
{
    int model_vertex_count;
    int entity_count;
};

void AnimationRender::render(Scene& scene)
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
        const int entity_count = static_cast<int>(model->entity_list.size());

        for (const auto& mesh_data : model->mesh_data_list)
        {
            model_vertex_count += static_cast<int>(mesh_data.vertices.size());
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
        render_buffers->animation_draw_parameters_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        parameter_list.size() * sizeof(int), parameter_list.data(),
        GL_STATIC_DRAW);

    
    shader->bind();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0,
        render_buffers->binding_poses_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1,
        render_buffers->bones_indices_weights_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2,
        render_buffers->bones_matrices_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3,
        render_buffers->dest_animation_vbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4,
        render_buffers->animation_draw_parameters_ssbo);

    int mesh_count = 0;
    int base_draw_parameter = 0;
    for (const auto& model : model_list)
    {
        if (model->entity_list.empty())
        {
            continue;
        }
        const RenderInfo& info = render_info.find(model->id)->second;

        mesh_count = static_cast<int>(model->mesh_data_list.size());
        shader->uniforms.set_uniform("base_draw_parameter", base_draw_parameter);
        glDispatchCompute(info.model_vertex_count, info.entity_count * mesh_count, 1);
        base_draw_parameter += info.entity_count * mesh_count;
    }

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    shader->unbind();
}
#endif
