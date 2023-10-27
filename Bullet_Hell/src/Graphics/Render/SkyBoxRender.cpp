#include "SkyBoxRender.h"

#include "GameLogic.h"
#include "ResourceCache.h"
#include "TextureResource.h"

#pragma region Shader code
const char fragment_shader_source[] = R"glsl(
#version 460

in vec2 texture_coordinates_out;
out vec4 fragment_color;

uniform vec4 diffuse;
uniform sampler2D texture_sampler;
uniform int has_texture;

void main()
{
    if (has_texture == 1) {
        fragment_color = texture(texture_sampler, texture_coordinates_out);
    } else {
        fragment_color = diffuse;
    }
}
)glsl";

const char vertex_shader_source[] = R"glsl(
#version 460

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texture_coordinates;

out vec2 texture_coordinates_out;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(position, 1.0);
    texture_coordinates_out = texture_coordinates;
}
)glsl";
#pragma endregion

SkyBoxRender::SkyBoxRender()
{
    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(vertex_shader_source,
        sizeof(vertex_shader_source), GL_VERTEX_SHADER);
    shader_modules.emplace_back(fragment_shader_source,
        sizeof(fragment_shader_source), GL_FRAGMENT_SHADER);
    shader_program = std::make_unique<ShaderProgram>(shader_modules);

    uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);
    create_uniforms();
}

void SkyBoxRender::render(const Scene& scene)
{
    const SkyBox& sky_box = scene.sky_box;

    shader_program->bind();

    uniforms_map->set_uniform("projection_matrix",
        scene.projection.projection_matrix);
    glm::mat4 view_matrix = scene.camera.view_matrix;
    //NOTE(ches) directly set transform to 0
    view_matrix[3][0] = 0;
    view_matrix[3][1] = 0;
    view_matrix[3][2] = 0;
    uniforms_map->set_uniform("view_matrix", view_matrix);
    uniforms_map->set_uniform("texture_sampler", 0);

    const auto& material = sky_box.model->mesh_data_list[0].material;
    uniforms_map->set_uniform("diffuse", material->diffuse_color);

    bool has_texture = false;
    if (material->texture_name != "")
    {
        glActiveTexture(GL_TEXTURE0);
        auto texture = load_texture(material->normal_map_name);
        texture->bind();
        has_texture = true;
    }

    uniforms_map->set_uniform("has_texture", has_texture ? 1 : 0);

    glBindVertexArray(sky_box.vao);

    uniforms_map->set_uniform("model_matrix", sky_box.entity->model_matrix);
    glDrawElements(GL_TRIANGLES, sky_box.index_count, GL_UNSIGNED_INT,
        nullptr);

    glBindVertexArray(0);
    shader_program->unbind();
}

void SkyBoxRender::create_uniforms()
{
    uniforms_map->create_uniform("projection_matrix");
    uniforms_map->create_uniform("view_matrix");
    uniforms_map->create_uniform("model_matrix");
    uniforms_map->create_uniform("diffuse");
    uniforms_map->create_uniform("texture_sampler");
    uniforms_map->create_uniform("has_texture");
}
