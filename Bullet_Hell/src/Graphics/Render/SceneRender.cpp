#include "../../Utilities/Globals.h"
#include "SceneRender.h"

const char fragment_shader_source[] = R"glsl(
#version 400

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

layout (std430, binding=1) buffer DrawElements {
    DrawElement draw_elements[];
};

layout (std430, binding=2) buffer Matrices {
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
