#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include "graphics/backend/opengl/render_buffers.h"

#include <cmath>
#include <cstdint>
#include <memory>
#include <set>

#include "glm/gtc/type_ptr.hpp"

#include "debugging/logger.h"
#include "graphics/graph/animation_resource.h"
#include "graphics/graph/material.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/graph/mesh_draw_data.h"
#include "graphics/scene/scene.h"

RenderBuffers::RenderBuffers()
	: vbo_list{}
	, buffers_populated{ false }
{
	
}

RenderBuffers::~RenderBuffers()
{
	cleanup();
}

void RenderBuffers::cleanup()
{
	if (!buffers_populated)
	{
		return;
	}
	buffers_populated = false;

	vbo_list.clear();
}

void RenderBuffers::define_vertex_attributes()
{
	
}

void RenderBuffers::load_animated_entity_buffers(const Scene& scene)
{
	
}

void RenderBuffers::load_animated_models(const Scene& scene)
{
	buffers_populated = true;
	const ModelList& model_list = scene.get_animated_model_list();

	load_binding_poses(model_list);
	load_bones_matrices_buffer(model_list);
	load_bones_indices_weights(model_list);
}

void RenderBuffers::load_binding_poses(const ModelList& models)
{

}

void RenderBuffers::load_bones_indices_weights(const ModelList& models)
{

}

void RenderBuffers::load_bones_matrices_buffer(const ModelList& models)
{

}


void RenderBuffers::load_static_models(const Scene& scene)
{
	
}

#endif
