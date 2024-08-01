#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/light_render.h"

#include <format>

#include "memory/memory_util.h"

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

#endif
