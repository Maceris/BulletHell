#include "../../Utilities/Globals.h"
#include "LightRender.h"

const char vertex_shader_source[] = R"glsl(
#version 460

layout (location=0) in vec3 position_in;
layout (location=1) in vec2 coordinate_in;

out vec2 texture_coordinate_out;

void main()
{
    texture_coordinate_out = position_in;
    gl_Position = vec4(coordinate_in, 1.0f);
}
)glsl";

const char fragment_shader_source[] = R"glsl(
#version 460

const int MAX_POINT_LIGHTS = 50;
const int MAX_SPOT_LIGHTS = 50;
const float SPECULAR_POWER = 10;
const int NUM_CASCADES = 3;
const float BIAS = 0.0005;
const float SHADOW_FACTOR = 0.25;

in vec2 texture_coordinate_out;
out vec4 fragment_color_out;

struct Attenuation
{
    float constant;
    float linear;
    float exponent;
};
struct AmbientLight
{
    float intensity;
    vec3 color;
};
struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    Attenuation attenuation;
};
struct SpotLight
{
    PointLight point_light;
    vec3 cone_direction;
    float cutoff;
};
struct DirectionalLight
{
    vec3 color;
    vec3 direction;
    float intensity;
};
struct Fog
{
    int enabled;
    vec3 color;
    float density;
};
struct CascadeShadow {
    mat4 projection_view_matrix;
    float split_distance;
};

uniform sampler2D albedo_sampler;
uniform sampler2D normal_sampler;
uniform sampler2D specular_sampler;
uniform sampler2D depth_sampler;

uniform mat4 inverse_projection_matrix;
uniform mat4 inverse_view_matrix;

uniform AmbientLight ambient_light;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform SpotLight spot_lights[MAX_SPOT_LIGHTS];
uniform DirectionalLight directional_light;
uniform Fog fog;
uniform CascadeShadow cascade_shadows[NUM_CASCADES];
uniform sampler2D shadow_map_0;
uniform sampler2D shadow_map_1;
uniform sampler2D shadow_map_2;

vec4 calculate_ambient(AmbientLight ambient_light, vec4 ambient) {
    return vec4(ambient_light.intensity * ambient_light.color, 1) * ambient;
}

vec4 calculate_light_color(vec4 diffuse, vec4 specular, float reflectance, vec3 light_color, float light_intensity, vec3 position, vec3 direction_to_light, vec3 normal) {
    vec4 diffuse_color = vec4(0, 0, 0, 1);
    vec4 specular_color = vec4(0, 0, 0, 1);

    // Diffuse Light
    float diffuse_factor = max(dot(normal, direction_to_light), 0.0);
    diffuse_color = diffuse * vec4(light_color, 1.0) * light_intensity * diffuse_factor;

    // Specular Light
    vec3 camera_direction = normalize(-position);
    vec3 direction_from_light = -direction_to_light;
    vec3 reflected_light = normalize(reflect(direction_from_light, normal));
    float specular_factor = max(dot(camera_direction, reflected_light), 0.0);
    specular_factor = pow(specular_factor, SPECULAR_POWER);
    specular_color = specular * light_intensity  * specular_factor * reflectance * vec4(light_color, 1.0);

    return (diffuse_color + specular_color);
}

vec4 calculate_point_light(vec4 diffuse, vec4 specular, float reflectance, PointLight light, vec3 position, vec3 normal) {
    vec3 direction_to_light = normalize(light.position - position);
    vec4 light_color = calculate_light_color(diffuse, specular, reflectance, light.color, light.intensity, position, direction_to_light, normal);

    // Apply Attenuation
    float distance = length(direction_to_light);
    float attenuationInv = light.attenuation.constant + light.attenuation.linear * distance +
    light.attenuation.exponent * distance * distance;
    return light_color / attenuationInv;
}

vec4 calculate_spot_light(vec4 diffuse, vec4 specular, float reflectance, SpotLight light, vec3 position, vec3 normal) {
    vec3 direction_from_light = -normalize(light.point_light.position - position);
    float spot_light_alpha = dot(direction_from_light, normalize(light.cone_direction));

    vec4 color = vec4(0, 0, 0, 0);

    if (spot_light_alpha > light.cutoff)
    {
        color = calculate_point_light(diffuse, specular, reflectance, light.point_light, position, normal);
        color *= (1.0 - (1.0 - spot_light_alpha)/(1.0 - light.cutoff));
    }
    return color;
}

vec4 calculate_directional_light(vec4 diffuse, vec4 specular, float reflectance, DirectionalLight light, vec3 position, vec3 normal) {
    return calculate_light_color(diffuse, specular, reflectance, light.color, light.intensity, position, normalize(light.direction), normal);
}

vec4 calculate_fog(vec3 pos, vec4 color, Fog fog, vec3 ambient_light, DirectionalLight directional_light) {
    vec3 fog_color = fog.color * (ambient_light + directional_light.color * directional_light.intensity);
    float distance = length(pos);
    float fog_factor = 1.0 / exp((distance * fog.density) * (distance * fog.density));
    fog_factor = clamp(fog_factor, 0.0, 1.0);

    vec3 resulting_color = mix(fog_color, color.xyz, fog_factor);
    return vec4(resulting_color.xyz, color.w);
}

float project_texture(vec4 shadow_coordinates, vec2 offset, int index) {
    float shadow = 1.0;

    if (shadow_coordinates.z > -1.0 && shadow_coordinates.z < 1.0) {
        float dist = 0.0;
        if (index == 0) {
            dist = texture(shadow_map_0, vec2(shadow_coordinates.xy + offset)).r;
        } else if (index == 1) {
            dist = texture(shadow_map_1, vec2(shadow_coordinates.xy + offset)).r;
        } else {
            dist = texture(shadow_map_2, vec2(shadow_coordinates.xy + offset)).r;
        }
        if (shadow_coordinates.w > 0 && dist < shadow_coordinates.z - BIAS) {
            shadow = SHADOW_FACTOR;
        }
    }
    return shadow;
}

float calculate_shadow(vec4 world_position, int index) {
    vec4 shadow_map_position = cascade_shadows[index].projection_view_matrix * world_position;
    float shadow = 1.0;
    vec4 shadow_coordinates = (shadow_map_position / shadow_map_position.w) * 0.5 + 0.5;
    shadow = project_texture(shadow_coordinates, vec2(0, 0), index);
    return shadow;
}

void main()
{
    vec4 albedo_sampler_value = texture(albedo_sampler, texture_coordinate_out);
    vec3 albedo = albedo_sampler_value.rgb;
    vec4 diffuse = vec4(albedo, 1);

    float reflectance = albedo_sampler_value.a;
    vec3 normal = normalize(2.0 * texture(normal_sampler, texture_coordinate_out).rgb  - 1.0);
    vec4 specular = texture(specular_sampler, texture_coordinate_out);

    // Retrieve position from depth
    float depth = texture(depth_sampler, texture_coordinate_out).x * 2.0 - 1.0;
    if (depth == 1) {
        discard;
    }
    vec4 clip = vec4(texture_coordinate_out.x * 2.0 - 1.0, texture_coordinate_out.y * 2.0 - 1.0, depth, 1.0);
    vec4 view_w = inverse_projection_matrix * clip;
    vec3 view_position = view_w.xyz / view_w.w;
    vec4 world_position = inverse_view_matrix * vec4(view_position, 1);

    vec4 diffucse_specular_component = calculate_directional_light(diffuse, specular, reflectance, directional_light, view_position, normal);

    int cascade_index;
    for (int i=0; i < NUM_CASCADES - 1; i++) {
        if (view_position.z < cascade_shadows[i].split_distance) {
            cascade_index = i + 1;
            break;
        }
    }
    float shadow_factor = calculate_shadow(world_position, cascade_index);

    for (int i=0; i < MAX_POINT_LIGHTS; i++) {
        if (point_lights[i].intensity > 0) {
            diffucse_specular_component += calculate_point_light(diffuse, specular, reflectance, point_lights[i], view_position, normal);
        }
    }

    for (int i=0; i < MAX_SPOT_LIGHTS; i++) {
        if (spot_lights[i].point_light.intensity > 0) {
            diffucse_specular_component += calculate_spot_light(diffuse, specular, reflectance, spot_lights[i], view_position, normal);
        }
    }
	
    vec4 ambient = calculate_ambient(ambient_light, diffuse);
    fragment_color_out = ambient + diffucse_specular_component;
    fragment_color_out.rgb = fragment_color_out.rgb * shadow_factor;

    if (fog.enabled == 1) {
        fragment_color_out = calculate_fog(view_position, fragment_color_out, fog, ambient_light.color, directional_light);
    }
}
)glsl";