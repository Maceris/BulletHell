#include "graphics/render/debug_render.h"

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

#pragma region Structs

#pragma pack(push,1)
struct Line
{
    glm::vec4 start;
    glm::vec4 end;

    Line(const glm::vec3& start, const glm::vec3& end)
        : start{ start, 1.0f }
        , end{ end, 1.0f }
    {}
    Line(const Line&) = default;
    Line& operator=(const Line&) = default;
    ~Line() = default;

    Line operator+(const glm::vec3& displacement) const
    {
        const glm::vec4 displace_vec4(displacement, 0.0f);
        return Line(start + displace_vec4, end + displace_vec4);
    }
};
#pragma pack(pop)

struct AABBLines
{
    std::vector<Line> lines;

    AABBLines(const glm::vec3& aabb_min, const glm::vec3& aabb_max)
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
    AABBLines(const AABBLines&) = default;
    AABBLines& operator=(const AABBLines&) = default;
    ~AABBLines() = default;
};
#pragma endregion

LineGroup::LineGroup()
    : vao{ 0 }
    , data{ 0 }
    , count{ 0 }
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &data);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, data);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        (const void*)0);
    glBindVertexArray(0);
}

LineGroup::~LineGroup()
{
    glDeleteBuffers(1, &data);
    glDeleteVertexArrays(1, &vao);
}

DebugRender::DebugRender()
{
    Resource frag("shaders/debug.frag");
    std::shared_ptr<ResourceHandle> frag_handle =
        g_game_logic->resource_cache->get_handle(&frag);
    LOG_ASSERT(frag_handle && "Cannot find debug fragment shader");

    Resource vert("shaders/debug.vert");
    std::shared_ptr<ResourceHandle> vert_handle =
        g_game_logic->resource_cache->get_handle(&vert);
    LOG_ASSERT(vert_handle && "Cannot find debug vertex shader");

    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(vert_handle->get_buffer(),
        vert_handle->get_size(), GL_VERTEX_SHADER);
    shader_modules.emplace_back(frag_handle->get_buffer(),
        frag_handle->get_size(), GL_FRAGMENT_SHADER);
    shader_program = std::make_unique<ShaderProgram>(shader_modules);

    uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);
    create_uniforms();
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

void DebugRender::render(const Scene& scene)
{
    shader_program->bind();
    glDepthMask(GL_FALSE);

    uniforms_map->set_uniform("projection_matrix",
        scene.projection.projection_matrix);
    uniforms_map->set_uniform("view_matrix", scene.camera.view_matrix);
    uniforms_map->set_uniform("line_color", RED);
    glBindVertexArray(map_lines.vao);
    glDrawArrays(GL_LINES, 0, map_lines.count * 2);

    update_AABBs(scene);
    uniforms_map->set_uniform("line_color", GREEN);
    glBindVertexArray(AABB_lines.vao);
    glDrawArrays(GL_LINES, 0, AABB_lines.count * 2);

    uniforms_map->set_uniform("line_color", BLUE);
    glBindVertexArray(hot_chunk_lines.vao);
    glDrawArrays(GL_LINES, 0, hot_chunk_lines.count * 2);
    
    uniforms_map->set_uniform("line_color", YELLOW);
    glBindVertexArray(cold_chunk_lines.vao);
    glDrawArrays(GL_LINES, 0, cold_chunk_lines.count * 2);

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    shader_program->unbind();
}

void DebugRender::update_AABBs(const Scene& scene)
{
    glBindBuffer(GL_ARRAY_BUFFER, AABB_lines.data);

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

    AABB_lines.count = static_cast<int>(lines.size());

    glBufferData(GL_ARRAY_BUFFER, AABB_lines.count * sizeof(Line),
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
    glBindBuffer(GL_ARRAY_BUFFER, map_lines.data);

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
                    + glm::vec3( 1.0f, 0.0f, -1.0f) * TILE_SCALE;
                glm::vec3 corner3 = start 
                    + glm::vec3( 1.0f, 0.0f,  1.0f) * TILE_SCALE;
                glm::vec3 corner4 = start 
                    + glm::vec3(-1.0f, 0.0f,  1.0f) * TILE_SCALE;

                lines.emplace_back(corner1, corner2);
                lines.emplace_back(corner2, corner3);
                lines.emplace_back(corner3, corner4);
                lines.emplace_back(corner4, corner1);
            }
        }
    }

    map_lines.count = static_cast<int>(lines.size());

    glBufferData(GL_ARRAY_BUFFER, map_lines.count * sizeof(Line),
        lines.data(), GL_STATIC_DRAW);
    lines.clear();

    std::vector<Line> hot_lines;
    add_chunks(g_game_logic->current_map->hot_cache, hot_lines);
    hot_chunk_lines.count = static_cast<int>(hot_lines.size());
    glBindBuffer(GL_ARRAY_BUFFER, hot_chunk_lines.data);
    glBufferData(GL_ARRAY_BUFFER, hot_chunk_lines.count * sizeof(Line),
        hot_lines.data(), GL_STATIC_DRAW);
    hot_lines.clear();

    std::vector<Line> cold_lines;
    add_chunks(g_game_logic->current_map->cold_cache, cold_lines);
    cold_chunk_lines.count = static_cast<int>(cold_lines.size());
    glBindBuffer(GL_ARRAY_BUFFER, cold_chunk_lines.data);
    glBufferData(GL_ARRAY_BUFFER, cold_chunk_lines.count * sizeof(Line),
        cold_lines.data(), GL_STATIC_DRAW);
    cold_lines.clear();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DebugRender::create_uniforms()
{
    uniforms_map->create_uniform("projection_matrix");
    uniforms_map->create_uniform("view_matrix");
    uniforms_map->create_uniform("line_color");
}
