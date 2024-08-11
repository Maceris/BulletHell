#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/debug_render.h"

#include <numbers>

#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "debugging/logger.h"
#include "graphics/graph/texture_resource.h"
#include "graphics/graph/cascade_shadow_slice.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/graph/model.h"
#include "graphics/scene/scene.h"
#include "main/game_logic.h"
#include "map/chunk.h"
#include "map/tile.h"
#include "resource_cache/resource_cache.h"
#include "utilities/math_util.h"

#include "glad.h"

constexpr glm::vec4 RED = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
constexpr glm::vec4 YELLOW = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
constexpr glm::vec4 GREEN = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
constexpr glm::vec4 BLUE = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

struct LineGroup::Data
{
    Data()
        : vao{ 0 }
        , data{ 0 }
        , count{ 0 }
    {}
    Data(const Data& data) = delete;
    Data& operator=(const Data& data) = delete;
    ~Data() = default;

    /// <summary>
    /// The VAO for this group.
    /// </summary>
    GLuint vao;
    /// <summary>
    /// The buffer handle for the line data.
    /// </summary>
    GLuint data;
    /// <summary>
    /// The number of lines. Notably, not the number of points like 
    /// glDrawArrays expects.
    /// </summary>
    int count;
};

LineGroup::LineGroup()
    : data{}
{
    glGenVertexArrays(1, &data->vao);
    glGenBuffers(1, &data->data);
    glBindVertexArray(data->vao);
    glBindBuffer(GL_ARRAY_BUFFER, data->data);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        (const void*)0);
    glBindVertexArray(0);
}

LineGroup::~LineGroup()
{
    glDeleteBuffers(1, &data->data);
    glDeleteVertexArrays(1, &data->vao);
}

Line Line::operator+(const glm::vec3& displacement) const
{
    const glm::vec4 displace_vec4(displacement, 0.0f);
    return Line(start + displace_vec4, end + displace_vec4);
}

AABBLines::AABBLines(const glm::vec3& aabb_min, const glm::vec3& aabb_max)
{
    //NOTE(ches) min corner
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z),
        glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z),
        glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_min.y, aabb_min.z),
        glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z)
    );

    //NOTE(ches) max corner
    lines.emplace_back(
        glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z),
        glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z),
        glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_max.x, aabb_max.y, aabb_max.z),
        glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z)
    );

    //NOTE(ches) The rest of the lines
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z),
        glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z),
        glm::vec3(aabb_min.x, aabb_max.y, aabb_max.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_min.y, aabb_max.z),
        glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_min.x, aabb_max.y, aabb_min.z),
        glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z),
        glm::vec3(aabb_max.x, aabb_max.y, aabb_min.z)
    );
    lines.emplace_back(
        glm::vec3(aabb_max.x, aabb_min.y, aabb_min.z),
        glm::vec3(aabb_max.x, aabb_min.y, aabb_max.z)
    );
}

glm::mat4 default_camera_view()
{
    const float TWO_PI = (float)(2 * std::numbers::pi);

    glm::vec3 position(-11.0f, 11.0f, 0.0f);
    glm::vec2 rotation(0.42f, 1.92f);
    rotation.x = MathUtil::clamp_float(rotation.x, glm::radians(-90.0f),
        glm::radians(90.0f));
    rotation.y = fmodf(rotation.y, TWO_PI);
    rotation.y = fmodf(rotation.y + TWO_PI, TWO_PI);

    glm::mat4 view_matrix(1.0f);
    view_matrix = glm::rotate(view_matrix, rotation.x,
        glm::vec3(1.0f, 0.0f, 0.0f));
    view_matrix = glm::rotate(view_matrix, rotation.y,
        glm::vec3(0.0f, 1.0f, 0.0f));
    view_matrix = glm::translate(view_matrix, -position);

    return view_matrix;
}

DebugRender::DebugRender(DebugInfo* debug_info)
    : debug_info{ debug_info }
{
    std::vector<Shader::Module> shader_modules;
    shader_modules.emplace_back("shaders/debug.frag",
        Shader::Type::FRAGMENT);
    shader_modules.emplace_back("shaders/debug.vert",
        Shader::Type::VERTEX);

    shader = ALLOC Shader(shader_modules);

    shader->uniforms.create_uniform("projection_matrix");
    shader->uniforms.create_uniform("view_matrix");
    shader->uniforms.create_uniform("line_color");
}

void DebugRender::render(Scene& scene)
{
    shader->bind();
    glDepthMask(GL_FALSE);

    shader->uniforms.set_uniform("projection_matrix",
        scene.projection.projection_matrix);
    shader->uniforms.set_uniform("view_matrix", scene.camera.view_matrix);
    shader->uniforms.set_uniform("line_color", RED);
    glBindVertexArray(debug_info->map_lines.data->vao);
    glDrawArrays(GL_LINES, 0, debug_info->map_lines.data->count * 2);

    update_AABBs(scene);
    shader->uniforms.set_uniform("line_color", GREEN);
    glBindVertexArray(debug_info->AABB_lines.data->vao);
    glDrawArrays(GL_LINES, 0, debug_info->AABB_lines.data->count * 2);

    shader->uniforms.set_uniform("line_color", BLUE);
    glBindVertexArray(debug_info->hot_chunk_lines.data->vao);
    glDrawArrays(GL_LINES, 0, debug_info->hot_chunk_lines.data->count * 2);

    shader->uniforms.set_uniform("line_color", YELLOW);
    glBindVertexArray(debug_info->cold_chunk_lines.data->vao);
    glDrawArrays(GL_LINES, 0, debug_info->cold_chunk_lines.data->count * 2);

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    shader->unbind();
}


void DebugRender::update_AABBs(const Scene& scene)
{
    glBindBuffer(GL_ARRAY_BUFFER, debug_info->AABB_lines.data->data);

    std::vector<Line> lines;
    for (auto& model : scene.get_model_list())
    {
        std::vector<AABBLines> boxes;
        for (auto& mesh_data : model->mesh_data_list)
        {
            boxes.emplace_back(mesh_data.aabb_min, mesh_data.aabb_max);
        }

        for (auto& entity : model->entity_list)
        {
            glm::vec3 position = entity->position;

            for (const auto& box : boxes)
            {
                for (const auto& line : box.lines)
                {
                    lines.push_back(line + position);
                }
            }
        }
    }

    debug_info->AABB_lines.data->count = static_cast<int>(lines.size());

    glBufferData(GL_ARRAY_BUFFER, debug_info->AABB_lines.data->count * sizeof(Line),
        lines.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/// <summary>
/// Add debug lines for all the chunks in the specified cache to the line list.
/// </summary>
/// <param name="cache">The cache to pull chunk information from.</param>
/// <param name="lines">Where to store debug lines.</param>
void add_chunks(const std::unordered_map<uint32_t, Chunk*>& cache,
    std::vector<Line>& lines)
{
    const float chunk_world_width = CHUNK_WIDTH * TILE_SCALE * 2;

    for (const auto& pair : cache)
    {
        ChunkCoordinates coordinates{ pair.first };

        glm::vec3 corner1{
            coordinates.x * chunk_world_width,
            0.0f,
            coordinates.z * chunk_world_width
        };

        glm::vec3 corner2 = corner1
            + glm::vec3(1.0f, 0.0f, 0.0f) * (TILE_SCALE * CHUNK_WIDTH * 2);
        glm::vec3 corner3 = corner1
            + glm::vec3(1.0f, 0.0f, 1.0f) * (TILE_SCALE * CHUNK_WIDTH * 2);
        glm::vec3 corner4 = corner1
            + glm::vec3(0.0f, 0.0f, 1.0f) * (TILE_SCALE * CHUNK_WIDTH * 2);

        lines.emplace_back(corner1, corner2);
        lines.emplace_back(corner2, corner3);
        lines.emplace_back(corner3, corner4);
        lines.emplace_back(corner4, corner1);

        glm::vec3 middle = (corner3 - corner1) * 0.5f + corner1;
        glm::vec3 end = middle + glm::vec3(0.0f, 1.0f, 0.0f);
        lines.emplace_back(middle, end);
    }
}
void DebugRender::update_lines(const Scene& scene)
{
    glBindBuffer(GL_ARRAY_BUFFER, debug_info->map_lines.data->data);

    std::vector<Line> lines;
    for (auto& chunk_pair : scene.chunk_contents)
    {
        for (auto& model_pair : chunk_pair.second->entities)
        {
            for (auto& entity : model_pair.second)
            {
                glm::vec3 start = entity->position;
                glm::vec3 end = start + glm::vec3(0.0f, 1.0f, 0.0f);
                lines.emplace_back(start, end);

                glm::vec3 corner1 = start
                    + glm::vec3(-1.0f, 0.0f, -1.0f) * TILE_SCALE;
                glm::vec3 corner2 = start
                    + glm::vec3(1.0f, 0.0f, -1.0f) * TILE_SCALE;
                glm::vec3 corner3 = start
                    + glm::vec3(1.0f, 0.0f, 1.0f) * TILE_SCALE;
                glm::vec3 corner4 = start
                    + glm::vec3(-1.0f, 0.0f, 1.0f) * TILE_SCALE;

                lines.emplace_back(corner1, corner2);
                lines.emplace_back(corner2, corner3);
                lines.emplace_back(corner3, corner4);
                lines.emplace_back(corner4, corner1);
            }
        }
    }

    debug_info->map_lines.data->count = static_cast<int>(lines.size());

    glBufferData(GL_ARRAY_BUFFER, debug_info->map_lines.data->count * sizeof(Line),
        lines.data(), GL_STATIC_DRAW);
    lines.clear();

    std::vector<Line> hot_lines;
    add_chunks(g_game_logic->current_map->hot_cache, hot_lines);
    debug_info->hot_chunk_lines.data->count = static_cast<int>(hot_lines.size());
    glBindBuffer(GL_ARRAY_BUFFER, debug_info->hot_chunk_lines.data->data);
    glBufferData(GL_ARRAY_BUFFER,
        debug_info->hot_chunk_lines.data->count * sizeof(Line),
        hot_lines.data(), GL_STATIC_DRAW);
    hot_lines.clear();

    std::vector<Line> cold_lines;
    add_chunks(g_game_logic->current_map->cold_cache, cold_lines);
    debug_info->cold_chunk_lines.data->count = static_cast<int>(cold_lines.size());
    glBindBuffer(GL_ARRAY_BUFFER, debug_info->cold_chunk_lines.data->data);
    glBufferData(GL_ARRAY_BUFFER,
        debug_info->cold_chunk_lines.data->count * sizeof(Line),
        cold_lines.data(), GL_STATIC_DRAW);
    cold_lines.clear();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#endif
