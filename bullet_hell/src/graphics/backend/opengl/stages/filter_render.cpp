#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/filter_render.h"

#include <format>

#include "debugging/logger.h"
#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/uniforms_map.h"
#include "graphics/graph/shader_program.h"
#include "graphics/scene/scene.h"
#include "main/game_logic.h"
#include "memory/memory_util.h"
#include "resource_cache/resource_cache.h"

#include "glad.h"

FilterRender::FilterRender(StageResource<Framebuffer>* scene_texture,
	StageResource<QuadMesh>* quad_mesh)
	: scene_texture{ scene_texture }
	, quad_mesh{ quad_mesh }
	, shader{ nullptr }
{
	set_filter("shaders/filters/default");
}

void FilterRender::render(Scene& scene)
{
	//NOTE(ches) We don't want to overwrite the depth buffer
	glDepthMask(GL_FALSE);

	shader->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, (*scene_texture)->handle);

	shader->uniforms.set_uniform("screen_texture", 0);

	glBindVertexArray((*quad_mesh)->vao);
	glDrawElements(GL_TRIANGLES, QUAD_MESH_VERTEX_COUNT, GL_UNSIGNED_INT,
		nullptr);

	shader->unbind();

	glDepthMask(GL_TRUE);
}

void FilterRender::set_filter(const std::string_view name)
{
	if (shader != nullptr)
	{
		safe_delete(shader);
	}

	std::vector<Shader::Module> shader_modules;
	shader_modules.emplace_back(std::format("{}.frag", name),
		Shader::Type::FRAGMENT);
	shader_modules.emplace_back(std::format("{}.vert", name),
		Shader::Type::VERTEX);

	shader = ALLOC Shader(shader_modules);

	shader->uniforms.create_uniform("screen_texture");
}

#endif
