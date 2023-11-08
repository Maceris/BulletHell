#include "Graphics/Render/SceneRender.h"

#include "glad.h"

#include "Debugging/Logger.h"
#include "Main/GameLogic.h"
#include "Graphics/Graph/TextureResource.h"
#include "Graphics/Render/RenderConstants.h"

#pragma region Shader code

const char fragment_shader_source[] = R"glsl(
#version 460

const int MAX_MATERIALS = 30;
const int MAX_TEXTURES = 16;

in vec3 normal_out;
in vec3 tangent_out;
in vec3 bitangent_out;
in vec2 texture_coordinate_out;
in vec4 view_position_out;
in vec4 world_position_out;
flat in uint material_index_out;

layout (location = 0) out vec4 albedo_buffer;
layout (location = 1) out vec4 normal_buffer;
layout (location = 2) out vec4 specular_buffer;

struct Material
{
    vec4 diffuse;
    vec4 specular;
    float reflectance;
    int normal_map_index;
    int texture_index;
};

uniform sampler2D texture_sampler[MAX_TEXTURES];
uniform Material materials[MAX_MATERIALS];

vec3 calculate_normal(int index, vec3 normal, vec3 tangent, vec3 bitangent, vec2 texture_coordinates) {
    mat3 tangent_bitangent_normal = mat3(tangent, bitangent, normal);
    vec3 new_normal = texture(texture_sampler[index], texture_coordinates).rgb;
    new_normal = normalize(new_normal * 2.0 - 1.0);
    new_normal = normalize(tangent_bitangent_normal * new_normal);
    return new_normal;
}

void main() {
    Material material = materials[material_index_out];
    vec4 texture_color = texture(texture_sampler[material.texture_index], texture_coordinate_out);
    vec4 diffuse = texture_color + material.diffuse;
    if (diffuse.a < 0.5) {
        discard;
    }
    vec4 specular = texture_color + material.specular;

    vec3 normal = normal_out;
    if (material.normal_map_index > 0) {
        normal = calculate_normal(material.normal_map_index, normal_out, tangent_out, bitangent_out, texture_coordinate_out);
    }

    albedo_buffer = vec4(diffuse.xyz, material.reflectance);
    normal_buffer = vec4(0.5 * normal + 0.5, 1.0);
    specular_buffer = specular;
}
)glsl";

const char vertex_shader_source[] = R"glsl(
#version 460

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 tangent;
layout (location=3) in vec3 bitangent;
layout (location=4) in vec2 texture_coordinate;

out vec3 normal_out;
out vec3 tangent_out;
out vec3 bitangent_out;
out vec2 texture_coordinate_out;
out vec4 view_position_out;
out vec4 world_position_out;
out uint material_index_out;

struct DrawElement
{
    int model_matrix_index;
    int material_index;
};

layout (std430, binding=0) buffer DrawElements {
    DrawElement draw_elements[];
};

layout (std430, binding=1) buffer Matrices {
	mat4 model_matrices[];
};

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main()
{
    vec4 initial_position = vec4(position, 1.0);
    vec4 initial_normal = vec4(normal, 0.0);
    vec4 initial_tangent = vec4(tangent, 0.0);
    vec4 initial_bitangent = vec4(bitangent, 0.0);

    uint index = gl_BaseInstance + gl_InstanceID;
    DrawElement draw_element = draw_elements[index];
    material_index_out = draw_element.material_index;
    mat4 model_matrix =  model_matrices[draw_element.model_matrix_index];
    mat4 model_view_matrix = view_matrix * model_matrix;
    world_position_out = model_matrix * initial_position;
    view_position_out = view_matrix * world_position_out;
    gl_Position = projection_matrix * view_position_out;
    normal_out = normalize(model_view_matrix * initial_normal).xyz;
    tangent_out = normalize(model_view_matrix * initial_tangent).xyz;
    bitangent_out = normalize(model_view_matrix * initial_bitangent).xyz;
    texture_coordinate_out = texture_coordinate;
}
)glsl";

#pragma endregion

SceneRender::SceneRender()
    : texture_bindings{}
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

void SceneRender::render(const Scene& scene, const RenderBuffers& render_buffers,
    const GBuffer& gBuffer, const CommandBuffers& command_buffers)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer.gBuffer_ID);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, gBuffer.width, gBuffer.height);
    glDisable(GL_BLEND);
    shader_program->bind();

    uniforms_map->set_uniform("projection_matrix", 
        scene.projection.projection_matrix);
    uniforms_map->set_uniform("view_matrix",
        scene.camera.view_matrix);

    glActiveTexture(GL_TEXTURE0);
    Texture::default_texture->bind();

    int next_texture = 1;
    for (const auto& pair : scene.model_map)
    {
        const auto& model = pair.second;
        for (const auto& mesh_data : model->mesh_data_list)
        {
            const auto& material = mesh_data.material;
            auto normal = texture_bindings.find(material->normal_map_name);
            LOG_ASSERT(normal != texture_bindings.end()
                && "We found an unknown texture");
            if (normal->second == next_texture)
            {
                auto texture = load_texture(material->normal_map_name);
                glActiveTexture(GL_TEXTURE0 + next_texture);
                texture->bind();
                uniforms_map->set_uniform("texture_sampler["
                    + std::to_string(next_texture) + "]", next_texture);
                ++next_texture;
            }

            auto texture = texture_bindings.find(material->texture_name);
            LOG_ASSERT(texture != texture_bindings.end()
                && "We found an unknown texture");
            if (texture->second == next_texture)
            {
                auto texture = load_texture(material->texture_name);
                glActiveTexture(GL_TEXTURE0 + next_texture);
                texture->bind();
                uniforms_map->set_uniform("texture_sampler["
                    + std::to_string(next_texture) + "]", next_texture);
                ++next_texture;
            }
        }
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DRAW_ELEMENT_BINDING,
        command_buffers.static_draw_element_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MODEL_MATRICES_BINDING,
        command_buffers.static_model_matrices_buffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
        command_buffers.static_command_buffer);
    glBindVertexArray(render_buffers.static_vao);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
        command_buffers.static_draw_count, 0);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, DRAW_ELEMENT_BINDING,
        command_buffers.animated_draw_element_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, MODEL_MATRICES_BINDING,
        command_buffers.animated_model_matrices_buffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
        command_buffers.animated_command_buffer);
    glBindVertexArray(render_buffers.animated_vao);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr,
        command_buffers.animated_draw_count, 0);

    glBindVertexArray(0);
    glEnable(GL_BLEND);
    shader_program->unbind();
}

void SceneRender::create_uniforms()
{
    uniforms_map->create_uniform("projection_matrix");
    uniforms_map->create_uniform("view_matrix");

    for (int i = 0; i < MAX_TEXTURES; ++i)
    {
        uniforms_map->create_uniform("texture_sampler[" + std::to_string(i) 
            + "]");
    }

    for (int i = 0; i < MAX_MATERIALS; ++i)
    {
        const std::string prefix = "materials[" + std::to_string(i) + "].";
        uniforms_map->create_uniform(prefix + "diffuse");
        uniforms_map->create_uniform(prefix + "specular");
        uniforms_map->create_uniform(prefix + "reflectance");
        uniforms_map->create_uniform(prefix + "normal_map_index");
        uniforms_map->create_uniform(prefix + "texture_index");
    }
}

void SceneRender::setup_materials_uniform(const Scene& scene)
{
    shader_program->bind();
    MaterialID next_ID = 0;

    int next_texture = 1;

    texture_bindings.clear();

    texture_bindings.emplace("", 0);

    for (const auto& pair : scene.model_map)
    {
        const auto& model = pair.second;
        for (const auto& mesh_data : model->mesh_data_list)
        {
            const auto& material = mesh_data.material;
            material->material_id = next_ID++;

            const std::string prefix = "materials[" 
                + std::to_string(material->material_id) + "].";

            uniforms_map->set_uniform(prefix + "diffuse",
                material->diffuse_color);
            uniforms_map->set_uniform(prefix + "specular",
                material->specular_color);
            uniforms_map->set_uniform(prefix + "reflectance",
                material->reflectance);

            // Default texture is 0
            int index = 0;
            if (material->texture_name != "")
            {
                auto result = texture_bindings.find(material->texture_name);
                if (result == texture_bindings.end()) {
                    texture_bindings.emplace(
                        std::make_pair(material->texture_name, next_texture));
                    index = next_texture;
                    ++next_texture;
                }
                else {
                    index = result->second;
                }
            }
            uniforms_map->set_uniform(prefix + "texture_index", index);

            index = 0;
            if (material->normal_map_name != "")
            {
                auto result = texture_bindings.find(material->normal_map_name);
                if (result == texture_bindings.end()) {
                    texture_bindings.emplace(
                        std::make_pair(material->normal_map_name, next_texture)
                    );
                    index = next_texture;
                    ++next_texture;
                }
                else {
                    index = result->second;
                }
            }
            uniforms_map->set_uniform(prefix + "normal_map_index", index);
        }
    }
    shader_program->unbind();
}
