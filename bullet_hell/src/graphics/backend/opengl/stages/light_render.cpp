#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/light_render.h"

#include <format>

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
#include "memory/memory_util.h"
#include "resource_cache/resource_cache.h"

#include "glad.h"

LightRender::LightRender(StageResource<CascadeShadows>* cascade_shadows,
	StageResource<Buffer>* point_lights,
	StageResource<Buffer>* spot_lights,
	StageResource<Framebuffer>* shadow_buffers,
	StageResource<Framebuffer>* gbuffer,
	StageResource<QuadMesh>* quad_mesh)
	: cascade_shadows{ cascade_shadows }
	, point_lights{ point_lights }
	, spot_lights{ spot_lights }
	, shadow_buffers{ shadow_buffers }
	, gbuffer{ gbuffer }
	, quad_mesh{ quad_mesh }
{
	std::vector<Shader::Module> shader_modules;
	shader_modules.emplace_back("shaders/lights.frag",
		Shader::Type::FRAGMENT);
	shader_modules.emplace_back("shaders/lights.vert",
		Shader::Type::VERTEX);

	shader = ALLOC Shader(shader_modules);
    create_uniforms();
}

void LightRender::create_uniforms()
{
    shader->uniforms.create_uniform("albedo_sampler");
    shader->uniforms.create_uniform("normal_sampler");
    shader->uniforms.create_uniform("specular_sampler");
    shader->uniforms.create_uniform("depth_sampler");
    shader->uniforms.create_uniform("inverse_projection_matrix");
    shader->uniforms.create_uniform("inverse_view_matrix");

    shader->uniforms.create_uniform("ambient_light.intensity");
    shader->uniforms.create_uniform("ambient_light.color");

    shader->uniforms.create_uniform("point_light_count");
    shader->uniforms.create_uniform("spot_light_count");

    shader->uniforms.create_uniform("directional_light.color");
    shader->uniforms.create_uniform("directional_light.direction");
    shader->uniforms.create_uniform("directional_light.intensity");

    shader->uniforms.create_uniform("fog.enabled");
    shader->uniforms.create_uniform("fog.color");
    shader->uniforms.create_uniform("fog.density");

    for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        shader->uniforms.create_uniform(std::format("shadow_map_{}", i));
        shader->uniforms.create_uniform(
            std::format("cascade_shadows[{}].projection_view_matrix", i));
        shader->uniforms.create_uniform(
            std::format("cascade_shadows[{}].split_distance", i));
    }
}

void LightRender::render(Scene& scene)
{
    shader->bind();
    update_lights(scene);

    int next_texture = 0;
    
    for (int i = 0; i < G_BUFFER_TOTAL_TEXTURES; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + next_texture);
        glBindTexture(GL_TEXTURE_2D, (*gbuffer)->textures[next_texture]);
        ++next_texture;
    }

    shader->uniforms.set_uniform("albedo_sampler", 0);
    shader->uniforms.set_uniform("normal_sampler", 1);
    shader->uniforms.set_uniform("specular_sampler", 2);
    shader->uniforms.set_uniform("depth_sampler", 3);

    const Fog& fog = scene.fog;
    shader->uniforms.set_uniform("fog.enabled", fog.active);
    shader->uniforms.set_uniform("fog.color", fog.color);
    shader->uniforms.set_uniform("fog.density", fog.density);

    auto& shadows = **cascade_shadows;
    for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        shader->uniforms.set_uniform("shadow_map_" + std::to_string(i),
            next_texture + i);
        shader->uniforms.set_uniform("cascade_shadows[" + std::to_string(i)
            + "].projection_view_matrix",
            shadows[i].projection_view_matrix);
        shader->uniforms.set_uniform("cascade_shadows[" + std::to_string(i)
            + "].split_distance", shadows[i].split_distance);
    }
    
    for (unsigned int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + next_texture + i);
        glBindTexture(GL_TEXTURE_2D, (*shadow_buffers)->textures[i]);
    }

    shader->uniforms.set_uniform("inverse_projection_matrix",
        scene.projection.inverse_projection_matrix);
    shader->uniforms.set_uniform("inverse_view_matrix",
        scene.camera.inverse_view_matrix);

    glBindVertexArray((*quad_mesh)->vao);
    glDrawElements(GL_TRIANGLES, QUAD_MESH_VERTEX_COUNT, GL_UNSIGNED_INT,
        nullptr);

    shader->unbind();
}



void LightRender::update_lights(const Scene& scene)
{
    const glm::mat4& view_matrix = scene.camera.view_matrix;

    //TODO(ches) BH-52 - include the SceneCluster lights as well

    const SceneLights& scene_lights = scene.scene_lights;
    const AmbientLight& ambient_light = scene_lights.ambient_light;
    shader->uniforms.set_uniform("ambient_light.intensity",
        ambient_light.intensity);
    shader->uniforms.set_uniform("ambient_light.color", ambient_light.color);

    const DirectionalLight& directional_light = scene_lights.directional_light;
    glm::vec4 adjusted(directional_light.direction, 0);
    adjusted = view_matrix * adjusted;
    glm::vec3 direction(adjusted.x, adjusted.y, adjusted.z);
    shader->uniforms.set_uniform("directional_light.color",
        directional_light.color);
    shader->uniforms.set_uniform("directional_light.direction",
        direction);
    shader->uniforms.set_uniform("directional_light.intensity",
        directional_light.intensity);

    setup_point_light_buffer(scene);
    setup_spot_light_buffer(scene);
}

void LightRender::setup_point_light_buffer(const Scene& scene)
{
    const std::vector<PointLight>& lights = scene.scene_lights.point_lights;
    const glm::mat4& view_matrix = scene.camera.view_matrix;

    LOG_ASSERT(lights.size() <= MAX_LIGHTS_SUPPORTED
        && "More point lights than supported");

    const unsigned int lights_to_render =
        std::min(MAX_LIGHTS_SUPPORTED, (int)lights.size());

    float* light_buffer = ALLOC float[lights_to_render * POINT_LIGHT_SIZE];

    const float padding = 0.0f;
    for (size_t i = 0; i < lights_to_render; ++i)
    {
        const PointLight& light = lights[i];
        glm::vec4 light_position{ light.position, 1 };
        light_position = view_matrix * light_position;
        light_buffer[i * POINT_LIGHT_SIZE + 0] = light_position.x;
        light_buffer[i * POINT_LIGHT_SIZE + 1] = light_position.y;
        light_buffer[i * POINT_LIGHT_SIZE + 2] = light_position.z;
        light_buffer[i * POINT_LIGHT_SIZE + 3] = padding;
        light_buffer[i * POINT_LIGHT_SIZE + 4] = light.color.r;
        light_buffer[i * POINT_LIGHT_SIZE + 5] = light.color.g;
        light_buffer[i * POINT_LIGHT_SIZE + 6] = light.color.b;
        light_buffer[i * POINT_LIGHT_SIZE + 7] = light.intensity;
        light_buffer[i * POINT_LIGHT_SIZE + 8] = light.attenuation.constant;
        light_buffer[i * POINT_LIGHT_SIZE + 9] = light.attenuation.linear;
        light_buffer[i * POINT_LIGHT_SIZE + 10] = light.attenuation.exponent;
        light_buffer[i * POINT_LIGHT_SIZE + 11] = padding;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, POINT_LIGHT_BINDING,
        (*point_lights)->handle);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
        (size_t)lights_to_render * POINT_LIGHT_SIZE * sizeof(float),
        light_buffer);

    safe_delete_array(light_buffer);

    shader->uniforms.set_uniform("point_light_count",
        static_cast<int>(lights_to_render));
}

void LightRender::setup_spot_light_buffer(const Scene& scene)
{
    const std::vector<SpotLight>& lights = scene.scene_lights.spot_lights;
    const glm::mat4& view_matrix = scene.camera.view_matrix;

    LOG_ASSERT(lights.size() <= MAX_LIGHTS_SUPPORTED
        && "More spot lights than supported");

    const unsigned int lights_to_render =
        std::min(MAX_LIGHTS_SUPPORTED, (int)lights.size());

    float* light_buffer = ALLOC float[lights_to_render * SPOT_LIGHT_SIZE];

    const float padding = 0.0f;
    for (size_t i = 0; i < lights_to_render; ++i)
    {
        const SpotLight& light = lights[i];
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
        (*spot_lights)->handle);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
        (size_t)lights_to_render * SPOT_LIGHT_SIZE * sizeof(float),
        light_buffer);

    safe_delete_array(light_buffer);

    shader->uniforms.set_uniform("spot_light_count",
        static_cast<int>(lights_to_render));
}

#endif
