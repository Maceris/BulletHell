#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/render/render.h"

#include <unordered_map>

#include "glm/gtc/type_ptr.hpp"

#include "debugging/logger.h"
#include "debugging/timer.h"
#include "graphics/window.h"
#include "graphics/graph/mesh_draw_data.h"
#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/scene/scene.h"
#include "utilities/opengl_util.h"

#include "glad.h"

Configuration Render::configuration;

Render::Render(const Window& window)
	: gBuffer{ window.width, window.height }
	, animation_render{}
	, gui_render{ window }
	, light_render{}
	, scene_render{}
	, shadow_render{}
	, debug_render{}
	, sky_box_render{}
	, filter_render{}
	, render_buffers{}
	, command_buffers{}
	, screen_FBO{ 0 }
	, screen_RBO_depth{ 0 }
	, screen_texture{ 0 }
{
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//NOTE(ches) Support for transparencies
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &screen_texture);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.width, window.height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &screen_RBO_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, screen_RBO_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, window.width,
		window.height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &screen_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, screen_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		screen_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, screen_RBO_depth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Render::~Render()
{
	glDeleteRenderbuffers(1, &screen_RBO_depth);
	glDeleteFramebuffers(1, &screen_FBO);
	glDeleteTextures(1, &screen_texture);
}

void Render::light_render_finish()
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render::light_render_start(const unsigned int width,
	const unsigned int height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, screen_FBO);
	glBindRenderbuffer(GL_RENDERBUFFER, screen_RBO_depth);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screen_texture);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
}

void calculate_materials(const Scene& scene, bool animated)
{
	int next_ID = 0;
	std::unordered_map<Material, int, MaterialHash> materials;

	const auto& model_list = animated
		? scene.get_animated_model_list()
		: scene.get_static_model_list();
	
	for (const auto& model : model_list)
	{
		for (const auto& mesh_data : model->mesh_data_list)
		{
			const std::shared_ptr<Material>& material = mesh_data.material;

			int material_index = 0;
			if (materials.contains(*material))
			{
				material_index = materials.find(*material)->second;
			}
			else
			{
				materials.insert(std::make_pair(*material, next_ID));
				material_index = next_ID;
				++next_ID;
				LOG_ASSERT(next_ID <= MAX_MATERIALS
					&& "We have more materials than we can bind in one draw call");
			}
			material->material_id = material_index;
		}
	}
}

void Render::recalculate_materials(const Scene& scene)
{
	const bool for_animated_models = true;
	const bool for_static_models = false;

	calculate_materials(scene, for_animated_models);
	calculate_materials(scene, for_static_models);
}

void Render::refresh_animated_data(Scene& scene)
{
	if (scene.animated_models_dirty)
	{
		render_buffers.load_animated_models(scene);
	}
	if (scene.animated_models_dirty || scene.animated_entities_dirty)
	{
		setup_animated_command_buffer(scene);
	}
	scene.animated_entities_dirty = false;
	scene.animated_models_dirty = false;
}

void Render::refresh_static_data(Scene& scene)
{
	if (scene.static_models_dirty)
	{
		render_buffers.load_static_models(scene);
	}
	if (scene.static_models_dirty || scene.static_entities_dirty)
	{
		setup_static_command_buffer(scene);
	}
	scene.static_entities_dirty = false;
	scene.static_models_dirty = false;
}

void Render::render(const Window& window, const Scene& scene)
{
	TIME_END("Last Frame");
	TIME_START("Last Frame");
	update_model_matrices(scene);

	TIME_START("Animation Render");
	animation_render.render(scene, render_buffers);
	TIME_END("Animation Render");

	TIME_START("Shadow Render");
	shadow_render.render(scene, render_buffers, command_buffers);
	TIME_END("Shadow Render");

	TIME_START("Scene Render");
#if _DEBUG
	if (Render::configuration.wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_TEXTURE_2D);
	}
#endif

	scene_render.render(scene, render_buffers, gBuffer, command_buffers);

#if _DEBUG
	if (Render::configuration.wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_TEXTURE_2D);
	}
#endif
	TIME_END("Scene Render");

	TIME_START("Light Render");
	light_render_start(window.width, window.height);
	light_render.render(scene, shadow_render, gBuffer);
	TIME_END("Light Render");

	TIME_START("Skybox Render");
	sky_box_render.render(scene);
	light_render_finish();
	TIME_END("Skybox Render");

	TIME_START("Filter Render");
	filter_render.render(scene, screen_texture);
	TIME_END("Filter Render");

#if _DEBUG
	if (Render::configuration.debug_lines)
	{
		TIME_START("Debug Render");
		debug_render.render(scene);
		TIME_END("Debug Render");
	}
#endif // _DEBUG

	TIME_START("Gui Render");
	gui_render.render(scene);
	TIME_END("Gui Render");
}

void Render::render_just_ui(const Window& window, const Scene& scene)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, window.width, window.height);

	TIME_START("Gui Render");
	gui_render.render(scene);
	TIME_END("Gui Render");
}

void Render::resize(const unsigned int width, const unsigned int height)
{
	gui_render.resize(width, height);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, screen_RBO_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, 
		height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Render::setup_all_data(Scene& scene)
{
	TIME_START("Updating Scene - Updating Data - Materials");
	recalculate_materials(scene);
	TIME_END("Updating Scene - Updating Data - Materials");

	TIME_START("Updating Scene - Updating Data - Static");
	if (scene.static_models_dirty || scene.static_entities_dirty)
	{
		refresh_static_data(scene);
	}
	TIME_END("Updating Scene - Updating Data - Static");

	TIME_START("Updating Scene - Updating Data - Animated");
	if (scene.animated_models_dirty || scene.animated_entities_dirty)
	{
		refresh_animated_data(scene);
	}
	TIME_END("Updating Scene - Updating Data - Animated");
	
#if _DEBUG
	TIME_START("Updating Scene - Updating Data - Debug Lines");
	if (Render::configuration.debug_lines)
	{
		debug_render.update_lines(scene);
	}
	TIME_END("Updating Scene - Updating Data - Debug Lines");
#endif
	scene.dirty = false;
}

void Render::setup_animated_command_buffer(const Scene& scene)
{
	const std::vector<std::shared_ptr<Model>>& model_list = 
		scene.get_animated_model_list();

	render_buffers.load_animated_entity_buffers(scene);

	size_t mesh_count = 0;
	size_t entity_count = 0;
	for (const auto& model : model_list)
	{
		mesh_count += model->mesh_draw_data_list.size();
		entity_count += model->entity_list.size();
	}

	std::map<const uint64_t, int> entity_index_map;

	float* model_matrices = ALLOC float[entity_count * 16];

	int entity_index = 0;
	for (const auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		for (const auto& entity : entities)
		{
			const float* matrix = static_cast<const float*>(
				glm::value_ptr(entity->model_matrix));
			for (size_t i = 0; i < 16; ++i)
			{
				model_matrices[static_cast<size_t>(entity_index) * 16 + i] 
					= matrix[i];
			}
			entity_index_map.emplace(
				std::make_pair(entity->entity_ID, entity_index));
			++entity_index;
		}
	}
	size_t data_size_in_bytes = entity_count * 16 * sizeof(float);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers.animated_model_matrices_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, model_matrices,
		GL_DYNAMIC_DRAW);
	safe_delete_array(model_matrices);

	int first_index = 0;
	int base_instance = 0;

	int command_buffer_index = 0;
	int draw_element_index = 0;
	const int COMMAND_SIZE = 5;
	const int DRAW_ELEMENT_SIZE = 2;

	const int padding = 0;

	int* command_buffer = ALLOC int[mesh_count * COMMAND_SIZE];
	int* draw_elements = ALLOC int[mesh_count * DRAW_ELEMENT_SIZE];
	for (const auto& model : model_list)
	{
		for (const auto& mesh_draw_data : model->mesh_draw_data_list)
		{
			// count
			command_buffer[command_buffer_index * COMMAND_SIZE + 0] =
				mesh_draw_data.indices;
			// instance count
			command_buffer[command_buffer_index * COMMAND_SIZE + 1] = 1;
			command_buffer[command_buffer_index * COMMAND_SIZE + 2] =
				first_index;
			// base vertex
			command_buffer[command_buffer_index * COMMAND_SIZE + 3] =
				mesh_draw_data.offset;
			command_buffer[command_buffer_index * COMMAND_SIZE + 4] =
				base_instance;

			first_index += mesh_draw_data.indices;
			++base_instance;
			++command_buffer_index;

			const auto& entity = mesh_draw_data.animated_mesh_draw_data.entity;

			const auto result = entity_index_map.find(entity->entity_ID);
			LOG_ASSERT(result != entity_index_map.end()
				&& "Entity ID not found in the index map");
			draw_elements[draw_element_index * DRAW_ELEMENT_SIZE] =
				result->second;
			draw_elements[draw_element_index * DRAW_ELEMENT_SIZE + 1] =
				mesh_draw_data.material;
			++draw_element_index;
		}
	}

	data_size_in_bytes = mesh_count * COMMAND_SIZE * sizeof(int);

	LOG_ASSERT(mesh_count <= UINT_MAX
		&& "We have more animated models than fit in an unsigned int");

	command_buffers.animated_draw_count = static_cast<unsigned int>(mesh_count);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
		command_buffers.animated_command_buffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, data_size_in_bytes, command_buffer,
		GL_STATIC_DRAW);
	safe_delete_array(command_buffer);

	data_size_in_bytes = mesh_count * DRAW_ELEMENT_SIZE * sizeof(int);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers.animated_draw_element_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, draw_elements,
		GL_STATIC_DRAW);
	safe_delete_array(draw_elements);
}

void Render::setup_static_command_buffer(const Scene& scene)
{
	const std::vector<std::shared_ptr<Model>>& model_list =
		scene.get_static_model_list();

	size_t mesh_count = 0;
	size_t draw_element_count = 0;
	size_t entity_count = 0;
	for (const auto& model : model_list)
	{
		mesh_count += model->mesh_draw_data_list.size();
		draw_element_count += model->entity_list.size()
			* model->mesh_draw_data_list.size();
		entity_count += model->entity_list.size();
	}

	std::map<const uint64_t, int> entity_index_map;

	float* model_matrices = ALLOC float[entity_count * 16];

	int entity_index = 0;
	for (const auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		for (const auto& entity : entities)
		{
			const float* matrix =  static_cast<const float*>(
				glm::value_ptr(entity->model_matrix));
			for (size_t i = 0; i < 16; ++i)
			{
				model_matrices[static_cast<size_t>(entity_index) * 16 + i]
					= matrix[i];
			}
			entity_index_map.emplace(
				std::make_pair(entity->entity_ID, entity_index));
			++entity_index;
		}
	}
	size_t data_size_in_bytes = entity_count * 16 * sizeof(float);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers.static_model_matrices_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, model_matrices,
		GL_DYNAMIC_DRAW);
	safe_delete_array(model_matrices);

	int first_index = 0;
	int base_instance = 0;

	int command_buffer_index = 0;
	int draw_element_index = 0;
	const int COMMAND_SIZE = 5;
	const int DRAW_ELEMENT_SIZE = 2;

	const int padding = 0;

	int* command_buffer = ALLOC int[mesh_count * COMMAND_SIZE];
	int* draw_elements = ALLOC int[draw_element_count * DRAW_ELEMENT_SIZE];
	for (const auto& model : model_list)
	{
		const EntityList& entities = model->entity_list;
		const int entity_count = static_cast<int>(entities.size());
		for (const auto& mesh_draw_data : model->mesh_draw_data_list)
		{
			// count
			command_buffer[command_buffer_index * COMMAND_SIZE + 0] =
				mesh_draw_data.indices;
			command_buffer[command_buffer_index * COMMAND_SIZE + 1] =
				entity_count;
			command_buffer[command_buffer_index * COMMAND_SIZE + 2] =
				first_index;
			// base vertex
			command_buffer[command_buffer_index * COMMAND_SIZE + 3] =
				mesh_draw_data.offset;
			command_buffer[command_buffer_index * COMMAND_SIZE + 4] =
				base_instance;

			first_index += mesh_draw_data.indices;
			base_instance += entity_count;
			++command_buffer_index;

			const int material_index = mesh_draw_data.material;
			for (const auto& entity : entities)
			{
				auto index = entity_index_map.find(entity->entity_ID);
				LOG_ASSERT(index != entity_index_map.end()
					&& "Our entity ID is missing");
				const auto id = index->second;
				draw_elements[draw_element_index * DRAW_ELEMENT_SIZE] =
					id;
				draw_elements[draw_element_index * DRAW_ELEMENT_SIZE + 1] =
					material_index;

				++draw_element_index;
			}
		}
	}
	LOG_ASSERT(mesh_count <= UINT_MAX
		&& "We have too more static models than fit in an unsigned int");

	data_size_in_bytes = mesh_count * COMMAND_SIZE * sizeof(int);

	command_buffers.static_draw_count = static_cast<unsigned int>(mesh_count);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
		command_buffers.static_command_buffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, data_size_in_bytes, command_buffer,
		GL_STATIC_DRAW);
	safe_delete_array(command_buffer);

	data_size_in_bytes = draw_element_count * DRAW_ELEMENT_SIZE * sizeof(int);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers.static_draw_element_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, draw_elements,
		GL_STATIC_DRAW);
	safe_delete_array(draw_elements);
}

void Render::update_model_buffer(
	const std::vector<std::shared_ptr<Model>> models, GLuint buffer_id)
{
	size_t entity_count = 0;
	for (const auto& model : models)
	{
		entity_count += model->entity_list.size();
	}

	float* model_matrices = ALLOC float[entity_count * 16];

	int entity_index = 0;
	for (const auto& model : models)
	{
		for (const auto& entity : model->entity_list)
		{
			const float* matrix = static_cast<const float*>(
				glm::value_ptr(entity->model_matrix));
			for (size_t i = 0; i < 16; ++i)
			{
				model_matrices[static_cast<size_t>(entity_index) * 16 + i]
					= matrix[i];
			}
			++entity_index;
		}
	}
	size_t data_size_in_bytes = entity_count * 16 * sizeof(float);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes,
		model_matrices, GL_DYNAMIC_DRAW);

	safe_delete_array(model_matrices);
}

void Render::update_model_matrices(const Scene& scene)
{
	ModelList animated_models = scene.get_animated_model_list();
	ModelList static_models = scene.get_static_model_list();

	GLuint animated_buffer = command_buffers.animated_model_matrices_buffer;
	GLuint static_buffer = command_buffers.static_model_matrices_buffer;

	update_model_buffer(animated_models, animated_buffer);
	update_model_buffer(static_models, static_buffer);
}

#endif
