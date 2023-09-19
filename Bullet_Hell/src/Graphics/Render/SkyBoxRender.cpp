#include "../../Utilities/Globals.h"
#include "SkyBoxRender.h"


const char fragment_shader_source[] = R"glsl(
#version 330

in vec2 texture_coordinates;
out vec4 fragment_color;

uniform vec4 diffuse;
uniform sampler2D texture_sampler;
uniform int has_texture;

void main()
{
    if (has_texture == 1) {
        fragment_color = texture(texture_sampler, texture_coordinates);
    } else {
        fragment_color = diffuse;
    }
}
)glsl";

const char vertex_shader_source[] = R"glsl(
#version 330

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
