#include "LightRender.h"

#include "CascadeShadowSlice.h"

#pragma region Shader code

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

#pragma endregion

constexpr auto MAX_POINT_LIGHTS = 50;
constexpr auto MAX_SPOT_LIGHTS = 50;

LightRender::LightRender()
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
}

void LightRender::render(const Scene& scene, const ShadowRender& shadow_render,
    const GBuffer& gBuffer)
{
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
    shadow_render.shadow_buffer->bind_textures(GL_TEXTURE0 + next_texture);

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

    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        const std::string prefix = "point_lights[" + std::to_string(i) + "].";
        uniforms_map->create_uniform(prefix + "position");
        uniforms_map->create_uniform(prefix + "color");
        uniforms_map->create_uniform(prefix + "intensity");
        uniforms_map->create_uniform(prefix + "attenuation.constant");
        uniforms_map->create_uniform(prefix + "attenuation.linear");
        uniforms_map->create_uniform(prefix + "attenuation.exponent");
    }

    for (int i = 0; i < MAX_SPOT_LIGHTS; ++i)
    {
        const std::string prefix = "spot_lights[" + std::to_string(i) + "].";
        uniforms_map->create_uniform(prefix + "point_light.position");
        uniforms_map->create_uniform(prefix + "point_light.color");
        uniforms_map->create_uniform(prefix + "point_light.intensity");
        uniforms_map->create_uniform(prefix + "point_light.attenuation.constant");
        uniforms_map->create_uniform(prefix + "point_light.attenuation.linear");
        uniforms_map->create_uniform(prefix + "point_light.attenuation.exponent");
        uniforms_map->create_uniform(prefix + "cone_direction");
        uniforms_map->create_uniform(prefix + "cutoff");
    }

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

    const auto& point_lights = scene_lights.point_lights;
    for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
    {
        const PointLight* point_light = i < point_lights.size() 
            ? &point_lights[i] : nullptr;

        const std::string prefix = "point_lights[" + std::to_string(i) + "].";
        update_point_light(point_light, prefix, view_matrix);
    }

    const auto& spot_lights = scene_lights.spot_lights;
    for (int i = 0; i < MAX_SPOT_LIGHTS; ++i)
    {
        const SpotLight* spot_light = i < spot_lights.size()
            ? &spot_lights[i] : nullptr;

        const std::string prefix = "spot_lights[" + std::to_string(i) + "].";
        update_spot_light(spot_light, prefix, view_matrix);
    }
}

void LightRender::update_point_light(const PointLight* point_light,
    const std::string& prefix, const glm::mat4& view_matrix)
{
    glm::vec3 position(0);
    glm::vec3 color(0);
    float intensity = 0.0f;
    float constant = 0.0f;
    float linear = 0.0f;
    float exponent = 0.0f;

    if (point_light != nullptr)
    {
        glm::vec4 temp(point_light->position, 1);
        temp = temp * view_matrix;
        position.x = temp.x;
        position.y = temp.y;
        position.z = temp.z;
        color = point_light->color;
        intensity = point_light->intensity;
        const auto& attenuation = point_light->attenuation;
        constant = attenuation.constant;
        linear = attenuation.linear;
        exponent = attenuation.exponent;
    }

    uniforms_map->set_uniform(prefix + "position", position);
    uniforms_map->set_uniform(prefix + "color", color);
    uniforms_map->set_uniform(prefix + "intensity", intensity);
    uniforms_map->set_uniform(prefix + "attenuation.constant", constant);
    uniforms_map->set_uniform(prefix + "attenuation.linear", linear);
    uniforms_map->set_uniform(prefix + "attenuation.exponent", exponent);
}

void LightRender::update_spot_light(const SpotLight* spot_light,
    const std::string& prefix, const glm::mat4& view_matrix)
{
    const PointLight* point_light = spot_light != nullptr 
        ? &spot_light->point_light : nullptr;
    glm::vec3 cone_direction(0);
    float cutoff = 0.0f;

    if (spot_light != nullptr)
    {
        cone_direction = spot_light->cone_direction;
        cutoff = spot_light->cut_off;
    }

    uniforms_map->set_uniform(prefix + "cone_direction", cone_direction);
    uniforms_map->set_uniform(prefix + "cutoff", cutoff);
    
    update_point_light(point_light, prefix + "point_light.", view_matrix);
}
