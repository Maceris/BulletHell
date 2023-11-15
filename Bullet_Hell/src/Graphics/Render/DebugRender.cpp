#include "Graphics/Render/Debugrender.h"

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
    : line_vao{ 0 }
    , line_data{ 0 }
{
    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(vertex_shader_source,
        sizeof(vertex_shader_source), GL_VERTEX_SHADER);
    shader_modules.emplace_back(fragment_shader_source,
        sizeof(fragment_shader_source), GL_FRAGMENT_SHADER);
    shader_program = std::make_unique<ShaderProgram>(shader_modules);

    uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);
    create_uniforms();

    glGenVertexArrays(1, &line_vao);
    glGenBuffers(1, &line_data);

    glBindVertexArray(line_vao);
    glBindBuffer(GL_ARRAY_BUFFER, line_data);

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

    glBindVertexArray(line_vao);
    
    glDrawArrays(GL_LINES, 0, line_count * 2);

    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
    shader_program->unbind();
}

void DebugRender::update_lines(const Scene& scene)
{
    glBindBuffer(GL_ARRAY_BUFFER, line_data);

    line_count = 0;

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
            }
        }
    }

    line_count = lines.size();

    glBufferData(GL_ARRAY_BUFFER, line_count * sizeof(Line), lines.data(), 
        GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DebugRender::create_uniforms()
{
    uniforms_map->create_uniform("projection_matrix");
    uniforms_map->create_uniform("view_matrix");
    uniforms_map->create_uniform("line_color");
}
