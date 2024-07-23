#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/render/light_render.h"

#include "debugging/logger.h"
#include "graphics/render_constants.h"
#include "graphics/graph/cascade_shadow_slice.h"
#include "graphics/graph/gbuffer.h"
#include "graphics/render/shadow_render.h"
#include "graphics/scene/scene.h"
#include "graphics/scene/lights/ambient_light.h"
#include "graphics/scene/lights/directional_light.h"
#include "graphics/scene/lights/point_light.h"
#include "graphics/scene/lights/scene_lights.h"
#include "graphics/scene/lights/spot_light.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

#include "glad.h"

LightRender::LightRender()
    : point_light_buffer{ 0 }
    , spot_light_buffer{ 0 }
{
    Resource frag("shaders/lights.frag");
    std::shared_ptr<ResourceHandle> frag_handle =
        g_game_logic->resource_cache->get_handle(&frag);
    LOG_ASSERT(frag_handle && "Cannot find lighting fragment shader");

    Resource vert("shaders/lights.vert");
    std::shared_ptr<ResourceHandle> vert_handle =
        g_game_logic->resource_cache->get_handle(&vert);
    LOG_ASSERT(vert_handle && "Cannot find lighting vertex shader");

    std::vector<ShaderModuleData> shader_modules;
    shader_modules.emplace_back(vert_handle->get_buffer(),
        vert_handle->get_size(), GL_VERTEX_SHADER);
    shader_modules.emplace_back(frag_handle->get_buffer(),
        frag_handle->get_size(), GL_FRAGMENT_SHADER);
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

    //TODO(ches) include the SceneCluster lights as well

    const SceneLights& scene_lights = scene.scene_lights;
    const AmbientLight& ambient_light = scene_lights.ambient_light;
    uniforms_map->set_uniform("ambient_light.intensity", 
        ambient_light.intensity);
    uniforms_map->set_uniform("ambient_light.color", ambient_light.color);

    const DirectionalLight& directional_light = scene_lights.directional_light;
    glm::vec4 adjusted(directional_light.direction, 0);
    adjusted = view_matrix * adjusted;
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
        light_position = view_matrix * light_position;
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

    uniforms_map->set_uniform("point_light_count", static_cast<int>(lights_to_render));
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
        light_position = view_matrix * light_position;
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
        glm::vec4 light_direction{ light.cone_direction, 1 };
        light_direction = view_matrix * light_direction;
        light_buffer[i * SPOT_LIGHT_SIZE + 12] = light_direction.x;
        light_buffer[i * SPOT_LIGHT_SIZE + 13] = light_direction.y;
        light_buffer[i * SPOT_LIGHT_SIZE + 14] = light_direction.z;
        light_buffer[i * SPOT_LIGHT_SIZE + 15] = light.cut_off;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SPOT_LIGHT_BINDING,
        spot_light_buffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
        (size_t)lights_to_render * SPOT_LIGHT_SIZE * sizeof(float),
        light_buffer);

    SAFE_DELETE_ARRAY(light_buffer);

    uniforms_map->set_uniform("spot_light_count", static_cast<int>(lights_to_render));
}
#endif
