#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/scene_render.h"

#include "debugging/logger.h"
#include "main/game_logic.h"
#include "graphics/render_constants.h"
#include "graphics/backend/base/pipeline_manager.h"
#include "graphics/backend/opengl/command_buffers.h"
#include "graphics/backend/opengl/render_buffers.h"
#include "graphics/frontend/texture.h"
#include "graphics/frontend/texture_loader.h"
#include "graphics/graph/texture_resource.h"
#include "graphics/graph/gbuffer.h"
#include "graphics/graph/material.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/scene/scene.h"
#include "main/game_logic.h"

SceneRender::SceneRender(StageResource<RenderBuffers>* render_buffers,
	StageResource<Framebuffer>* gbuffer,
	StageResource<CommandBuffers>* command_buffers,
	StageResource<Texture>* default_texture)
	: render_buffers{ render_buffers }
	, gbuffer{ gbuffer }
	, command_buffers{ command_buffers }
	, default_texture{ default_texture }
{
	std::vector<Shader::Module> shader_modules;
	shader_modules.emplace_back("shaders/scene.frag",
		Shader::Type::FRAGMENT);
	shader_modules.emplace_back("shaders/scene.vert",
		Shader::Type::VERTEX);

	shader = ALLOC Shader(shader_modules);
	create_uniforms();
}


void SceneRender::create_uniforms()
{
    shader->uniforms.create_uniform("projection_matrix");
    shader->uniforms.create_uniform("view_matrix");

    for (int i = 0; i < MAX_TEXTURES; ++i)
    {
        shader->uniforms.create_uniform(std::format("texture_sampler[{}]", i));
    }

    for (int i = 0; i < MAX_MATERIALS; ++i)
    {
        shader->uniforms.create_uniform(std::format("materials[{}].diffuse", i));
        shader->uniforms.create_uniform(std::format("materials[{}].specular", i));
        shader->uniforms.create_uniform(std::format("materials[{}].reflectance", i));
        shader->uniforms.create_uniform(std::format("materials[{}].normal_map_index", i));
        shader->uniforms.create_uniform(std::format("materials[{}].texture_index", i));
    }
}

#endif
