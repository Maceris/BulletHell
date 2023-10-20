#include "Render.h"

#include "gtc/type_ptr.hpp"

Render::Render(const Window& window)
	: gBuffer{ window.width, window.height }
	, animation_render{}
	, gui_render{ window }
	, light_render{}
	, scene_render{}
	, shadow_render{}
	, sky_box_render{}
	, filter_render{}
	, render_buffers{}
	, command_buffers{}
	, buffers_populated{ false }
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

void Render::recalculate_materials(const Scene& scene)
{
	scene_render.setup_materials_uniform(scene);
}

void Render::render(const Window& window, const Scene& scene)
{
	update_model_matrices(scene);

	animation_render.render(scene, render_buffers);
	shadow_render.render(scene, render_buffers, command_buffers);

	//TODO(ches) support wireframe?

	scene_render.render(scene, render_buffers, gBuffer, command_buffers);

	light_render_start(window.width, window.height);
	light_render.render(scene, shadow_render, gBuffer);
	sky_box_render.render(scene);
	light_render_finish();

	filter_render.render(scene, screen_texture);
	gui_render.render(scene);
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

void Render::setup_data(const Scene& scene)
{
	if (buffers_populated.exchange(false))
	{
		render_buffers.cleanup();
		command_buffers.cleanup();
	}
	render_buffers.load_static_models(scene);
	render_buffers.load_animated_models(scene);
	scene_render.setup_materials_uniform(scene);
	setup_animated_command_buffer(scene);
	setup_static_command_buffer(scene);
	buffers_populated = true;
}

void Render::setup_animated_command_buffer(const Scene& scene)
{
	std::vector<std::shared_ptr<Model>> model_list;
	
	for (const auto& pair : scene.model_map)
	{
		if (pair.second->is_animated())
		{
			model_list.push_back(pair.second);
		}
	}

	int mesh_count = 0;
	int draw_element_count = 0;
	int entity_count = 0;
	for (const auto& model : model_list)
	{
		mesh_count += model->mesh_draw_data_list.size();
		draw_element_count += model->entity_list.size()
			* model->mesh_draw_data_list.size();
		entity_count += model->entity_list.size();
	}

	std::map<uint64_t, int> entity_index_map;

	float* model_matrices = ALLOC float[entity_count * 16];

	int entity_index = 0;
	for (const auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		for (const auto& entity : entities)
		{
			auto matrix = glm::value_ptr(entity->model_matrix);
			std::copy(matrix, matrix + 16, 
				model_matrices + (entity_index * 16));
			entity_index_map.emplace(
				std::pair(entity->entity_ID, entity_index));
			++entity_index;
		}
	}
	size_t data_size_in_bytes =
		static_cast<size_t>(entity_count) * 16 * sizeof(float);

	glGenBuffers(1, &command_buffers.animated_model_matrices_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers.animated_model_matrices_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, model_matrices,
		GL_STATIC_DRAW);
	SAFE_DELETE_ARRAY(model_matrices);

	int first_index = 0;
	int base_instance = 0;

	int draw_element_index = 0;
	const int COMMAND_SIZE = 5;
	const int DRAW_ELEMENT_SIZE = 2;

	int* command_buffer = ALLOC int[mesh_count * COMMAND_SIZE];
	int* draw_elements = ALLOC int[draw_element_count * DRAW_ELEMENT_SIZE];
	for (const auto& model : model_list)
	{
		for (const auto& mesh_draw_data : model->mesh_draw_data_list)
		{
			// count
			command_buffer[base_instance * COMMAND_SIZE] = 
				mesh_draw_data.vertices;
			// instance count
			command_buffer[base_instance * COMMAND_SIZE + 1] = 1;
			command_buffer[base_instance * COMMAND_SIZE + 2] = first_index;
			// base vertex
			command_buffer[base_instance * COMMAND_SIZE + 3] = 
				mesh_draw_data.offset;
			command_buffer[base_instance * COMMAND_SIZE + 4] = base_instance;
			++base_instance;

			auto& entity = mesh_draw_data.animated_mesh_draw_data.entity;
			//NOTE(ches) it should (tm) be in the map
			draw_elements[draw_element_index * DRAW_ELEMENT_SIZE] =
				entity_index_map.find(entity->entity_ID)->second;
			draw_elements[draw_element_index * DRAW_ELEMENT_SIZE + 1] =
				mesh_draw_data.material;
			++draw_element_index;
		}
	}

	data_size_in_bytes = static_cast<size_t>(mesh_count) 
		* COMMAND_SIZE * sizeof(int);

	command_buffers.animated_draw_count = mesh_count;

	glGenBuffers(1, &command_buffers.animated_command_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers.animated_command_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, command_buffer,
		GL_STATIC_DRAW);
	SAFE_DELETE_ARRAY(command_buffer);

	data_size_in_bytes = static_cast<size_t>(draw_element_count) 
		* DRAW_ELEMENT_SIZE * sizeof(int);

	glGenBuffers(1, &command_buffers.animated_draw_element_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers.animated_draw_element_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, draw_elements,
		GL_STATIC_DRAW);
	SAFE_DELETE_ARRAY(draw_elements);
}

void Render::setup_static_command_buffer(const Scene& scene)
{
	std::vector<std::shared_ptr<Model>> model_list;

	for (const auto& pair : scene.model_map)
	{
		if (!pair.second->is_animated())
		{
			model_list.push_back(pair.second);
		}
	}

	int mesh_count = 0;
	int draw_element_count = 0;
	int entity_count = 0;
	for (const auto& model : model_list)
	{
		mesh_count += model->mesh_draw_data_list.size();
		draw_element_count += model->entity_list.size()
			* model->mesh_draw_data_list.size();
		entity_count += model->entity_list.size();
	}

	std::map<uint64_t, int> entity_index_map;

	float* model_matrices = ALLOC float[entity_count * 16];

	int entity_index = 0;
	for (const auto& model : model_list)
	{
		EntityList& entities = model->entity_list;
		for (const auto& entity : entities)
		{
			auto matrix = glm::value_ptr(entity->model_matrix);
			std::copy(matrix, matrix + 16,
				model_matrices + (entity_index * 16));
			entity_index_map.emplace(
				std::pair(entity->entity_ID, entity_index));
			++entity_index;
		}
	}
	size_t data_size_in_bytes =
		static_cast<size_t>(entity_count) * 16 * sizeof(float);

	glGenBuffers(1, &command_buffers.static_model_matrices_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers.static_model_matrices_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, model_matrices,
		GL_STATIC_DRAW);
	SAFE_DELETE_ARRAY(model_matrices);

	int first_index = 0;
	int base_instance = 0;

	int draw_element_index = 0;
	const int COMMAND_SIZE = 5;
	const int DRAW_ELEMENT_SIZE = 2;

	int* command_buffer = ALLOC int[mesh_count * COMMAND_SIZE];
	int* draw_elements = ALLOC int[draw_element_count * DRAW_ELEMENT_SIZE];
	for (const auto& model : model_list)
	{
		const EntityList& entities = model->entity_list;
		const int entity_count = (int) entities.size();
		for (const auto& mesh_draw_data : model->mesh_draw_data_list)
		{
			// count
			command_buffer[base_instance * COMMAND_SIZE] =
				mesh_draw_data.vertices;
			command_buffer[base_instance * COMMAND_SIZE + 1] = entity_count;
			command_buffer[base_instance * COMMAND_SIZE + 2] = first_index;
			// base vertex
			command_buffer[base_instance * COMMAND_SIZE + 3] =
				mesh_draw_data.offset;
			command_buffer[base_instance * COMMAND_SIZE + 4] = base_instance;

			first_index += mesh_draw_data.vertices;
			base_instance += entity_count;

			const MaterialID material_index = mesh_draw_data.material;
			auto& entity = mesh_draw_data.animated_mesh_draw_data.entity;
			for (const auto& entity : entities)
			{
				//NOTE(ches) it should (tm) be in the map
				draw_elements[draw_element_index * DRAW_ELEMENT_SIZE] =
					entity_index_map.find(entity->entity_ID)->second;
				draw_elements[draw_element_index * DRAW_ELEMENT_SIZE + 1] =
					material_index;
				++draw_element_index;
			}
		}
	}

	data_size_in_bytes = static_cast<size_t>(mesh_count)
		* COMMAND_SIZE * sizeof(int);

	command_buffers.animated_draw_count = mesh_count;

	glGenBuffers(1, &command_buffers.static_command_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers.static_command_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, command_buffer,
		GL_STATIC_DRAW);
	SAFE_DELETE_ARRAY(command_buffer);

	data_size_in_bytes = static_cast<size_t>(draw_element_count)
		* DRAW_ELEMENT_SIZE * sizeof(int);

	glGenBuffers(1, &command_buffers.static_draw_element_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers.static_draw_element_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, draw_elements,
		GL_STATIC_DRAW);
	SAFE_DELETE_ARRAY(draw_elements);
}

void Render::update_model_buffer(const std::vector<std::shared_ptr<Model>> models,
	GLuint buffer_id)
{
	//TODO(ches) complete this
}

void Render::update_model_matrices(const Scene& scene)
{
	//TODO(ches) complete this
}