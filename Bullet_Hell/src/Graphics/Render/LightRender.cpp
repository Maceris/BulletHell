#include "LightRender.h"

#include "CascadeShadowSlice.h"
#include "RenderConstants.h"

#pragma region Shader code

const char vertex_shader_source[] = R"glsl(
#version 460

layout (location=0) in vec3 position_in;
layout (location=1) in vec2 coordinate_in;

out vec2 texture_coordinate;

void main()
{
    texture_coordinate = coordinate_in;
    gl_Position = vec4(position_in, 1.0f);
}
)glsl";

const char fragment_shader_source[] = R"glsl(
#version 460

const float SPECULAR_POWER = 10;
const int NUM_CASCADES = 3;
const float BIAS = 0.0005;
const float SHADOW_FACTOR = 0.25;

in vec2 texture_coordinate;
out vec4 fragment_color_out;

struct Attenuation
{
    float constant;
    float linear;
    float exponent;
    float padding;
};
struct AmbientLight
{
    float intensity;
    vec3 color;
};
struct PointLight {
    vec3 position;
    float padding;
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

layout (std430, binding=0) readonly buffer PointLights {
    PointLight point_lights[];

};

layout (std430, binding=1) readonly buffer SpotLights {
    SpotLight spot_lights[];
};

uniform AmbientLight ambient_light;
uniform int point_light_count;
uniform int spot_light_count;
uniform DirectionalLight directional_light;
uniform Fog fog;
uniform CascadeShadow cascade_shadows[NUM_CASCADES];
uniform sampler2D shadow_map_0;
uniform sampler2D shadow_map_1;
uniform sampler2D shadow_map_2;

vec4 calculate_ambient(AmbientLight ambient_light, vec4 ambient) {
    return vec4(ambient_light.intensity * ambient_light.color, 1) * ambient;
}

vec4 calculate_light_color(vec4 diffuse, vec4 specular, float reflectance, vec3 light_color, float light_intensity, vec3 view_position, vec3 direction_to_light, vec3 normal) {
    vec4 diffuse_color = vec4(0, 0, 0, 1);
    vec4 specular_color = vec4(0, 0, 0, 1);

    // Diffuse Light
    float diffuse_factor = max(dot(normal, direction_to_light), 0.0);
    diffuse_color = diffuse * vec4(light_color, 1.0) * light_intensity * diffuse_factor;

    // Specular Light
    vec3 camera_direction = normalize(-view_position);
    vec3 direction_from_light = -direction_to_light;
    vec3 reflected_light = normalize(reflect(direction_from_light, normal));
    float specular_factor = max(dot(camera_direction, reflected_light), 0.0);
    specular_factor = pow(specular_factor, SPECULAR_POWER);
    specular_color = specular * light_intensity  * specular_factor * reflectance * vec4(light_color, 1.0);

    return (diffuse_color + specular_color);
}

vec4 calculate_point_light(vec4 diffuse, vec4 specular, float reflectance, PointLight light, vec3 view_position, vec3 normal) {
    vec3 direction_to_light = normalize(light.position - view_position);
    vec4 light_color = calculate_light_color(diffuse, specular, reflectance, light.color, light.intensity, view_position, direction_to_light, normal);

    // Apply Attenuation
    float distance = length(direction_to_light);
    float attenuationInv = light.attenuation.constant + light.attenuation.linear * distance +
    light.attenuation.exponent * distance * distance;
    return light_color / attenuationInv;
}

vec4 calculate_spot_light(vec4 diffuse, vec4 specular, float reflectance, SpotLight light, vec3 view_position, vec3 normal) {
    vec3 direction_from_light = -normalize(light.point_light.position - view_position);
    float spot_light_alpha = dot(direction_from_light, normalize(light.cone_direction));

    vec4 color = vec4(0, 0, 0, 0);

    if (spot_light_alpha > light.cutoff)
    {
        color = calculate_point_light(diffuse, specular, reflectance, light.point_light, view_position, normal);
        color *= (1.0 - (1.0 - spot_light_alpha)/(1.0 - light.cutoff));
    }
    return color;
}

vec4 calculate_directional_light(vec4 diffuse, vec4 specular, float reflectance, DirectionalLight light, vec3 view_position, vec3 normal) {
    return calculate_light_color(diffuse, specular, reflectance, light.color, light.intensity, view_position, normalize(light.direction), normal);
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
    vec4 albedo_sampler_value = texture(albedo_sampler, texture_coordinate);
    vec3 albedo = albedo_sampler_value.rgb;
    vec4 diffuse = vec4(albedo, 1);

    float reflectance = albedo_sampler_value.a;
    vec3 normal = normalize(2.0 * texture(normal_sampler, texture_coordinate).rgb  - 1.0);
    vec4 specular = texture(specular_sampler, texture_coordinate);

    // Retrieve position from depth
    float depth = texture(depth_sampler, texture_coordinate).x * 2.0 - 1.0;
    if (depth == 1) {
        discard;
    }
    vec4 clip = vec4(texture_coordinate.x * 2.0 - 1.0, texture_coordinate.y * 2.0 - 1.0, depth, 1.0);
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

    for (int i=0; i < point_light_count; i++) {
        if (point_lights[i].intensity > 0) {
            diffucse_specular_component += calculate_point_light(diffuse, specular, reflectance, point_lights[i], view_position, normal);
        }
    }

    for (int i=0; i < spot_light_count; i++) {
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

#pragma endregion

LightRender::LightRender()
    : point_light_buffer{ 0 }
    , spot_light_buffer{ 0 }
{
    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(vertex_shader_source,
        sizeof(vertex_shader_source), GL_VERTEX_SHADER);
    shader_modules.emplace_back(fragment_shader_source,
        sizeof(fragment_shader_source), GL_FRAGMENT_SHADER);
    shader_program = std::make_unique<ShaderProgram>(shader_modules);
    
    quad_mesh = std::make_unique<QuadMesh>();

    uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);
    create_uniforms();
    initialize_SSBOs();
}

void LightRender::initialize_SSBOs()
{
    glGenBuffers(1, &point_light_buffer);
    glGenBuffers(1, &spot_light_buffer);

    float* point_light_float_buffer =
        ALLOC float[MAX_LIGHTS_SUPPORTED * POINT_LIGHT_SIZE];
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POINT_LIGHT_BINDING,
        point_light_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        MAX_LIGHTS_SUPPORTED * POINT_LIGHT_SIZE * sizeof(float),
        point_light_float_buffer, GL_STATIC_DRAW);

    SAFE_DELETE_ARRAY(point_light_float_buffer);

    float* spot_light_float_buffer =
        ALLOC float[MAX_LIGHTS_SUPPORTED * SPOT_LIGHT_SIZE];
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SPOT_LIGHT_BINDING,
        spot_light_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        MAX_LIGHTS_SUPPORTED * SPOT_LIGHT_SIZE * sizeof(float),
        spot_light_float_buffer, GL_STATIC_DRAW);

    SAFE_DELETE_ARRAY(spot_light_float_buffer);

}

void LightRender::render(const Scene& scene, ShadowRender& shadow_render,
    const GBuffer& gBuffer)
{
    shader_program->bind();
    update_lights(scene);

    int next_texture = 0;
    if (gBuffer.texture_IDs != nullptr)
    {
        for (int i = 0; i < G_BUFFER_TOTAL_TEXTURES; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + next_texture);
            glBindTexture(GL_TEXTURE_2D, gBuffer.texture_IDs[next_texture]);
            ++next_texture;
        }
    }

    uniforms_map->set_uniform("albedo_sampler", 0);
    uniforms_map->set_uniform("normal_sampler", 1);
    uniforms_map->set_uniform("specular_sampler", 2);
    uniforms_map->set_uniform("depth_sampler", 3);

    const Fog& fog = scene.fog;
    uniforms_map->set_uniform("fog.enabled", fog.active);
    uniforms_map->set_uniform("fog.color", fog.color);
    uniforms_map->set_uniform("fog.density", fog.density);

    auto& cascade_shadows = shadow_render.cascade_shadows;
    for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        uniforms_map->set_uniform("shadow_map_" + std::to_string(i), 
            next_texture + i);
        uniforms_map->set_uniform("cascade_shadows[" + std::to_string(i) 
            + "].projection_view_matrix", 
            cascade_shadows[i].projection_view_matrix);
        uniforms_map->set_uniform("cascade_shadows[" + std::to_string(i)
            + "].split_distance", cascade_shadows[i].split_distance);
    }
    glActiveTexture(GL_TEXTURE0 + next_texture);
    shadow_render.shadow_buffer.bind_textures(GL_TEXTURE0 + next_texture);

    uniforms_map->set_uniform("inverse_projection_matrix",
        scene.projection.inverse_projection_matrix);
    uniforms_map->set_uniform("inverse_view_matrix",
        scene.camera.inverse_view_matrix);

    glBindVertexArray(quad_mesh->vao);
    glDrawElements(GL_TRIANGLES, QUAD_MESH_VERTEX_COUNT, GL_UNSIGNED_INT, 
        nullptr);

    shader_program->unbind();
}

void LightRender::create_uniforms()
{
    uniforms_map->create_uniform("albedo_sampler");
    uniforms_map->create_uniform("normal_sampler");
    uniforms_map->create_uniform("specular_sampler");
    uniforms_map->create_uniform("depth_sampler");
    uniforms_map->create_uniform("inverse_projection_matrix");
    uniforms_map->create_uniform("inverse_view_matrix");

    uniforms_map->create_uniform("ambient_light.intensity");
    uniforms_map->create_uniform("ambient_light.color");

    uniforms_map->create_uniform("point_light_count");
    uniforms_map->create_uniform("spot_light_count");

    uniforms_map->create_uniform("directional_light.color");
    uniforms_map->create_uniform("directional_light.direction");
    uniforms_map->create_uniform("directional_light.intensity");

    uniforms_map->create_uniform("fog.enabled");
    uniforms_map->create_uniform("fog.color");
    uniforms_map->create_uniform("fog.density");

    for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        uniforms_map->create_uniform("shadow_map_" + std::to_string(i));
        uniforms_map->create_uniform("cascade_shadows[" + std::to_string(i) 
            + "].projection_view_matrix");
        uniforms_map->create_uniform("cascade_shadows[" + std::to_string(i)
            + "].split_distance");
    }
}

void LightRender::update_lights(const Scene& scene)
{
    const glm::mat4& view_matrix = scene.camera.view_matrix;

    const SceneLights& scene_lights = scene.scene_lights;

    const AmbientLight& ambient_light = scene_lights.ambient_light;
    uniforms_map->set_uniform("ambient_light.intensity", 
        ambient_light.intensity);
    uniforms_map->set_uniform("ambient_light.color", ambient_light.color);

    const DirectionalLight& directional_light = scene_lights.directional_light;
    glm::vec4 adjusted(directional_light.direction, 0);
    adjusted = adjusted * view_matrix;
    glm::vec3 direction(adjusted.x, adjusted.y, adjusted.z);
    uniforms_map->set_uniform("directional_light.color",
        directional_light.color);
    uniforms_map->set_uniform("directional_light.direction",
        direction);
    uniforms_map->set_uniform("directional_light.intensity",
        directional_light.intensity);

    setup_point_light_buffer(scene);
    setup_spot_light_buffer(scene);
}

void LightRender::setup_point_light_buffer(const Scene& scene)
{
    const std::vector<PointLight>& point_lights = 
        scene.scene_lights.point_lights;
    const glm::mat4& view_matrix = scene.camera.view_matrix;

    LOG_ASSERT(point_lights.size() <= MAX_LIGHTS_SUPPORTED
        && "More point lights than supported");

    const unsigned int lights_to_render = 
        std::min(MAX_LIGHTS_SUPPORTED, (int) point_lights.size());

    float* light_buffer = ALLOC float[lights_to_render * POINT_LIGHT_SIZE];

    const float padding = 0.0f;
    for (size_t i = 0; i < lights_to_render; ++i)
    {
        const PointLight& light = point_lights[i];
        glm::vec4 light_position{ light.position, 1 };
        light_position = light_position * view_matrix;
        light_buffer[i * POINT_LIGHT_SIZE +  0] = light_position.x;
        light_buffer[i * POINT_LIGHT_SIZE +  1] = light_position.y;
        light_buffer[i * POINT_LIGHT_SIZE +  2] = light_position.z;
        light_buffer[i * POINT_LIGHT_SIZE +  3] = padding;
        light_buffer[i * POINT_LIGHT_SIZE +  4] = light.color.r;
        light_buffer[i * POINT_LIGHT_SIZE +  5] = light.color.g;
        light_buffer[i * POINT_LIGHT_SIZE +  6] = light.color.b;
        light_buffer[i * POINT_LIGHT_SIZE +  7] = light.intensity;
        light_buffer[i * POINT_LIGHT_SIZE +  8] = light.attenuation.constant;
        light_buffer[i * POINT_LIGHT_SIZE +  9] = light.attenuation.linear;
        light_buffer[i * POINT_LIGHT_SIZE + 10] = light.attenuation.exponent;
        light_buffer[i * POINT_LIGHT_SIZE + 11] = padding;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER,POINT_LIGHT_BINDING, 
        point_light_buffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
        (size_t) lights_to_render * POINT_LIGHT_SIZE * sizeof(float),
        light_buffer);

    SAFE_DELETE_ARRAY(light_buffer);

    uniforms_map->set_uniform("point_light_count", lights_to_render);
}

void LightRender::setup_spot_light_buffer(const Scene& scene)
{
    const std::vector<SpotLight>& spot_lights =
        scene.scene_lights.spot_lights;
    const glm::mat4& view_matrix = scene.camera.view_matrix;

    LOG_ASSERT(spot_lights.size() <= MAX_LIGHTS_SUPPORTED
        && "More spot lights than supported");

    const unsigned int lights_to_render =
        std::min(MAX_LIGHTS_SUPPORTED, (int)spot_lights.size());

    float* light_buffer = ALLOC float[lights_to_render * SPOT_LIGHT_SIZE];

    const float padding = 0.0f;
    for (size_t i = 0; i < lights_to_render; ++i)
    {
        const SpotLight& light = spot_lights[i];
        glm::vec4 light_position{ light.point_light.position, 1 };
        light_position = light_position * view_matrix;
        light_buffer[i * SPOT_LIGHT_SIZE + 0] = light_position.x;
        light_buffer[i * SPOT_LIGHT_SIZE + 1] = light_position.y;
        light_buffer[i * SPOT_LIGHT_SIZE + 2] = light_position.z;
        light_buffer[i * SPOT_LIGHT_SIZE + 3] = padding;
        light_buffer[i * SPOT_LIGHT_SIZE + 4] = light.point_light.color.r;
        light_buffer[i * SPOT_LIGHT_SIZE + 5] = light.point_light.color.g;
        light_buffer[i * SPOT_LIGHT_SIZE + 6] = light.point_light.color.b;
        light_buffer[i * SPOT_LIGHT_SIZE + 7] = light.point_light.intensity;
        light_buffer[i * SPOT_LIGHT_SIZE + 8] = light.point_light.attenuation.constant;
        light_buffer[i * SPOT_LIGHT_SIZE + 9] = light.point_light.attenuation.linear;
        light_buffer[i * SPOT_LIGHT_SIZE + 10] = light.point_light.attenuation.exponent;
        light_buffer[i * SPOT_LIGHT_SIZE + 11] = padding;
        light_buffer[i * SPOT_LIGHT_SIZE + 12] = light.cone_direction.x;
        light_buffer[i * SPOT_LIGHT_SIZE + 13] = light.cone_direction.y;
        light_buffer[i * SPOT_LIGHT_SIZE + 14] = light.cone_direction.z;
        light_buffer[i * SPOT_LIGHT_SIZE + 15] = light.cut_off;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SPOT_LIGHT_BINDING,
        spot_light_buffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
        (size_t)lights_to_render * SPOT_LIGHT_SIZE * sizeof(float),
        light_buffer);

    SAFE_DELETE_ARRAY(light_buffer);

    uniforms_map->set_uniform("point_light_count", lights_to_render);
}
