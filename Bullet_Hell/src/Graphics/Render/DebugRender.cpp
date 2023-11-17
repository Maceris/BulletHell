#include "Graphics/Render/DebugRender.h"

#include "glad.h"

#include "Graphics/Graph/TextureResource.h"
#include "Main/GameLogic.h"
#include "ResourceCache/ResourceCache.h"

#pragma region Shader code
const char fragment_shader_source[] = R"glsl(
#version 460

out vec4 fragment_color;
uniform vec4 line_color;

void main()
{
    fragment_color = line_color;
}
)glsl";

const char vertex_shader_source[] = R"glsl(
#version 460

layout (location=0) in vec4 position;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main()
{
    gl_Position = projection_matrix * view_matrix * position;
}
)glsl";
#pragma endregion

constexpr glm::vec4 RED = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
constexpr glm::vec4 GREEN = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
constexpr glm::vec4 BLUE = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

#pragma pack(push,1)
struct Line
{
    glm::vec4 start;
    glm::vec4 end;

    Line(glm::vec3 start, glm::vec3 end)
        : start{ start, 1.0f }
        , end{ end, 1.0f }
    {}
    Line(const Line&) = default;
    Line& operator=(const Line&) = default;
    ~Line() = default;
};
#pragma pack(pop)

DebugRender::DebugRender()
    : map_line_vao{ 0 }
    , map_line_data{ 0 }
    , frustrum_vao{ 0 }
    , frustrum_data{ 0 }
{
    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(vertex_shader_source,
        sizeof(vertex_shader_source), GL_VERTEX_SHADER);
    shader_modules.emplace_back(fragment_shader_source,
        sizeof(fragment_shader_source), GL_FRAGMENT_SHADER);
    shader_program = std::make_unique<ShaderProgram>(shader_modules);

    uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);
    create_uniforms();

    glGenVertexArrays(1, &map_line_vao);
    glGenBuffers(1, &map_line_data);
    glBindVertexArray(map_line_vao);
    glBindBuffer(GL_ARRAY_BUFFER, map_line_data);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 
        (const void*)0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &frustrum_vao);
    glGenBuffers(1, &frustrum_data);
    glBindVertexArray(frustrum_vao);
    glBindBuffer(GL_ARRAY_BUFFER, frustrum_data);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
        (const void*)0);
    glBindVertexArray(0);
}

void DebugRender::render(const Scene& scene)
{
    shader_program->bind();
    glDepthMask(GL_FALSE);

    uniforms_map->set_uniform("projection_matrix",
        scene.projection.projection_matrix);
    uniforms_map->set_uniform("view_matrix", scene.camera.view_matrix);
    uniforms_map->set_uniform("line_color", RED);

    glBindVertexArray(map_line_vao);
    glDrawArrays(GL_LINES, 0, map_line_count * 2);

    update_frustrums();
    glBindVertexArray(frustrum_vao);
    const int line_group = (4 * 3 + 3) * 2;
    glDrawArrays(GL_LINES, 0, line_group);
    uniforms_map->set_uniform("line_color", GREEN);
    glDrawArrays(GL_LINES, line_group, line_group);
    uniforms_map->set_uniform("line_color", BLUE);
    glDrawArrays(GL_LINES, line_group * 2, line_group);

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    shader_program->unbind();
}

void DebugRender::update_frustrums()
{
    glBindBuffer(GL_ARRAY_BUFFER, frustrum_data);

    std::vector<Line> frustrum_lines;
    for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        Frustrum& frustrum = CascadeShadowSlice::cached_frustrums[i];
        frustrum_lines.emplace_back(frustrum.corners[0], frustrum.corners[1]);
        frustrum_lines.emplace_back(frustrum.corners[1], frustrum.corners[2]);
        frustrum_lines.emplace_back(frustrum.corners[2], frustrum.corners[3]);
        frustrum_lines.emplace_back(frustrum.corners[3], frustrum.corners[0]);

        frustrum_lines.emplace_back(frustrum.corners[4], frustrum.corners[5]);
        frustrum_lines.emplace_back(frustrum.corners[5], frustrum.corners[6]);
        frustrum_lines.emplace_back(frustrum.corners[6], frustrum.corners[7]);
        frustrum_lines.emplace_back(frustrum.corners[7], frustrum.corners[4]);

        frustrum_lines.emplace_back(frustrum.corners[0], frustrum.corners[4]);
        frustrum_lines.emplace_back(frustrum.corners[1], frustrum.corners[5]);
        frustrum_lines.emplace_back(frustrum.corners[2], frustrum.corners[6]);
        frustrum_lines.emplace_back(frustrum.corners[3], frustrum.corners[7]);

        glm::vec3 center = frustrum.center;
        frustrum_lines.emplace_back(
            center - glm::vec3(1.0f, 0.0f, 0.0f),
            center + glm::vec3(1.0f, 0.0f, 0.0f)
        );
        frustrum_lines.emplace_back(
            center - glm::vec3(0.0f, 1.0f, 0.0f),
            center + glm::vec3(0.0f, 1.0f, 0.0f)
        );
        frustrum_lines.emplace_back(
            center - glm::vec3(0.0f, 0.0f, 1.0f),
            center + glm::vec3(0.0f, 0.0f, 1.0f)
        );
    }

    frustrum_line_count = frustrum_lines.size();
    glBufferData(GL_ARRAY_BUFFER, frustrum_line_count * sizeof(Line),
        frustrum_lines.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DebugRender::update_lines(const Scene& scene)
{
    glBindBuffer(GL_ARRAY_BUFFER, map_line_data);

    std::vector<Line> map_lines;
    for (auto& chunk_pair : scene.chunk_contents)
    {
        for (auto& model_pair : chunk_pair.second->entities)
        {
            for (auto& entity : model_pair.second)
            {
                glm::vec3 start = entity->position;
                glm::vec3 end = start + glm::vec3(0.0f, 1.0f, 0.0f);
                map_lines.emplace_back(start, end);
            }
        }
    }

    map_line_count = map_lines.size();

    glBufferData(GL_ARRAY_BUFFER, map_line_count * sizeof(Line), 
        map_lines.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DebugRender::create_uniforms()
{
    uniforms_map->create_uniform("projection_matrix");
    uniforms_map->create_uniform("view_matrix");
    uniforms_map->create_uniform("line_color");
}
