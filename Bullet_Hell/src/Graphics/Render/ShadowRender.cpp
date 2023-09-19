#include "../../Utilities/Globals.h"
#include "ShadowRender.h"

const char vertex_shader_source[] = R"glsl(
#version 460

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec3 tangent;
layout (location=3) in vec3 bitangent;
layout (location=4) in vec2 texture_coordinates;

struct DrawElement
{
    int model_matrix_index;
    int material_index;
};

uniform mat4 projection_view_matrix;

layout (std430, binding=1) buffer DrawElements {
    DrawElement draw_elements[];
};

layout (std430, binding=2) buffer Matrices {
	mat4 model_matrices[];
};

void main()
{
    vec4 initial_position = vec4(position, 1.0);
    uint index = gl_BaseInstance + gl_InstanceID;
	DrawElement draw_element = draw_elements[index];
    mat4 model_matrix = model_matrices[draw_element.model_matrix_index];
    gl_Position = projection_view_matrix * model_matrix * initial_position;
}
)glsl";