#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/render/filter_render.h"

#include "debugging/logger.h"
#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/graph/shader_program.h"
#include "graphics/graph/uniforms_map.h"
#include "graphics/scene/scene.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

#include "glad.h"

FilterRender::FilterRender()
{
	quad_mesh = std::make_unique<QuadMesh>();
	bool filter_set = set_filter("shaders/filters/default");
	LOG_ASSERT(filter_set);
}

void FilterRender::render(const Scene& scene, const GLuint screen_texture)
{
	// We don't want to overwrite the depth buffer
	glDepthMask(GL_FALSE);

	shader->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screen_texture);

	uniforms_map->set_uniform("screen_texture", 0);

	glBindVertexArray(quad_mesh->vao);
	glDrawElements(GL_TRIANGLES, QUAD_MESH_VERTEX_COUNT, GL_UNSIGNED_INT, 
		nullptr);

	shader->unbind();

	glDepthMask(GL_TRUE);
}

bool FilterRender::set_filter(const std::string name)
{
	Resource frag(name + ".frag");
	std::shared_ptr<ResourceHandle> frag_handle = 
		g_game_logic->resource_cache->get_handle(&frag);

	if (!frag_handle)
	{
		LOG_ERROR("Cannot find filter " + name + ".frag");
		return false;
	}

	Resource vert(name + ".vert");
	std::shared_ptr<ResourceHandle> vert_handle =
		g_game_logic->resource_cache->get_handle(&vert);

	if (!vert_handle)
	{
		LOG_ERROR("Cannot find filter " + name + ".vert");
		return false;
	}

	std::vector<ShaderModuleData> module_data;

	const char* frag_source = frag_handle->get_buffer();
	module_data.emplace_back(frag_source, frag_handle->get_size(), 
		GL_FRAGMENT_SHADER);
	
	const char* vert_source = vert_handle->get_buffer();
	module_data.emplace_back(vert_source, vert_handle->get_size(),
		GL_VERTEX_SHADER);

	shader = std::make_shared<ShaderProgram>(module_data);
	uniforms_map = std::make_unique<UniformsMap>(shader->program_id);

	uniforms_map->create_uniform("screen_texture");

	return true;
}
#endif
