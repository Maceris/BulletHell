#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/base/pipeline_manager.h"

#include <map>

#include "glm/gtc/type_ptr.hpp"

#include "debugging/logger.h"
#include "debugging/timer.h"
#include "graphics/render_constants.h"
#include "graphics/scene/scene.h"
#include "graphics/backend/opengl/stages/animation_render.h"
#include "graphics/backend/opengl/stages/debug_render.h"
#include "graphics/backend/opengl/stages/filter_render.h"
#include "graphics/backend/opengl/stages/framebuffer_transition.h"
#include "graphics/backend/opengl/stages/gui_render.h"
#include "graphics/backend/opengl/stages/light_render.h"
#include "graphics/backend/opengl/stages/model_matrix_update.h"
#include "graphics/backend/opengl/stages/scene_render.h"
#include "graphics/backend/opengl/stages/shadow_render.h"
#include "graphics/backend/opengl/stages/skybox_render.h"
#include "graphics/frontend/instance.h"
#include "memory/memory_util.h"

#include "glad.h"

Texture* PipelineManager::default_texture = nullptr;

/// <summary>
/// Generates new gbuffer and return the pointer to it.
/// </summary>
/// <param name="width">The width of the buffer in pixels.</param>
/// <param name="height">The height of the buffer in pixels.</param>
/// <returns>The newly allocated pointer.</returns>
Framebuffer* generate_gbuffer(unsigned int width, unsigned int height);

/// <summary>
/// Generates new render buffers and return the pointer to it.
/// </summary>
/// <param name="width">The width of the buffer in pixels.</param>
/// <param name="height">The height of the buffer in pixels.</param>
/// <returns>The newly allocated pointer.</returns>
Framebuffer* generate_screen_texture(unsigned int width, unsigned int height);

/// <summary>
/// Generates new shadow buffers and return the pointer to them.
/// </summary>
Framebuffer* generate_shadow_buffers();

struct PipelineManager::Data
{
	Data(const Window& window);
	Data(const Data&) = delete;
	Data& operator=(const Data&) = delete;
	~Data();

	StageResource<Buffer> point_lights;
	StageResource<Buffer> spot_lights;
	StageResource<CascadeShadows> cascade_shadows;
	StageResource<CommandBuffers> command_buffers;
	StageResource<Framebuffer> back_buffer;
	StageResource<Framebuffer> gbuffer;
	StageResource<Framebuffer> screen_texture;
	StageResource<Framebuffer> shadow_buffer;
	StageResource<GuiMesh> gui_mesh;
	StageResource<QuadMesh> quad_mesh;
	StageResource<RenderBuffers> render_buffers;
	StageResource<SkyBox> skybox;
	DebugInfo* const debug_info;

	unsigned int cached_height;
	unsigned int cached_width;

	AnimationRender animation_render;
	DebugRender debug_render;
	FramebufferTransition back_buffer_binding;
	FramebufferTransition screen_texture_binding;
	FilterRender filter_render;
	GuiRender gui_render;
	GuiRenderStandalone gui_render_standalone;
	LightRender light_render;
	ModelMatrixUpdate model_matrix_update;
	SceneRender scene_render;
	SceneRenderWireframe scene_render_wireframe;
	ShadowRender shadow_render;
	SkyboxRender skybox_render;

	std::map<RenderConfig, Pipeline*> pipelines;
};

PipelineManager::Data::Data(const Window& window)
	: point_lights{ ALLOC Buffer(Buffer::Type::SHADER_STORAGE) }
	, spot_lights{ ALLOC Buffer(Buffer::Type::SHADER_STORAGE) }
	, cascade_shadows{ ALLOC CascadeShadows() }
	, command_buffers{ ALLOC CommandBuffers() }
	, gbuffer{ generate_gbuffer(window.width, window.height) }
	, back_buffer{ ALLOC Framebuffer(0, window.width, window.height,
		std::vector<TextureHandle>()) }
	, screen_texture{ generate_screen_texture(window.width, window.height) }
	, shadow_buffer{ generate_shadow_buffers() }
	, gui_mesh{ ALLOC GuiMesh() }
	, quad_mesh{ ALLOC QuadMesh() }
	, render_buffers{ ALLOC RenderBuffers() }
	, skybox{ ALLOC SkyBox() }
	, debug_info{ ALLOC DebugInfo() }
	, cached_width{ window.width }
	, cached_height{ window.height }
	, animation_render{ &render_buffers }
	, back_buffer_binding{ &back_buffer, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA }
	, debug_render { debug_info }
	, screen_texture_binding{ &screen_texture, GL_ONE, GL_ONE }
	, filter_render{ &screen_texture, &quad_mesh }
	, gui_render{ window, &gui_mesh }
	, gui_render_standalone{ window, &gui_mesh }
	, light_render{ &cascade_shadows, &point_lights, &spot_lights,
		&shadow_buffer, &gbuffer, &quad_mesh }
	, model_matrix_update{ &command_buffers }
	, scene_render{ &render_buffers, &gbuffer, &command_buffers,
		&default_texture }
	, scene_render_wireframe{ &render_buffers, &gbuffer, &command_buffers,
		&default_texture }
	, shadow_render{ &render_buffers, &cascade_shadows, &shadow_buffer,
		&command_buffers }
	, skybox_render{ &skybox }
{}

PipelineManager::Data::~Data()
{
	//TODO(ches) delete all data
	for (auto& [key, pipeline] : pipelines)
	{
		safe_delete(pipeline);
	}
	pipelines.clear();
}

template <typename T>
	requires QueueDeletable<T>
void delete_resource(T* resource, DeletionQueue* const deletion_queue)
{
	if (resource != nullptr)
	{
		deletion_queue->add(resource);
		resource = nullptr;
	}
}

Framebuffer* generate_gbuffer(unsigned int width, unsigned int height)
{
	const int TEXTURE_COUNT = 4;

	GLuint buffer_id;
	glGenFramebuffers(1, &buffer_id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, buffer_id);

	GLuint texture_ids[TEXTURE_COUNT];
	glGenTextures(TEXTURE_COUNT, texture_ids);

	GLenum buffers[TEXTURE_COUNT - 1]{};
	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
		GLenum attachment_type;
		if (i == TEXTURE_COUNT - 1)
		{
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_DEPTH_COMPONENT32F,
				width,
				height,
				0,
				GL_DEPTH_COMPONENT,
				GL_FLOAT,
				nullptr
			);
			attachment_type = GL_DEPTH_ATTACHMENT;
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGBA32F,
				width,
				height,
				0,
				GL_RGBA,
				GL_FLOAT,
				nullptr
			);
			attachment_type = GL_COLOR_ATTACHMENT0 + i;
			buffers[i] = attachment_type;
		}
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, GL_TEXTURE_2D,
			texture_ids[i], 0);
	}

	glDrawBuffers(TEXTURE_COUNT - 1, buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<TextureHandle> texture_handles;
	for (int i = 0; i < TEXTURE_COUNT; ++i)
	{
		texture_handles.push_back(static_cast<TextureHandle>(texture_ids[i]));
	}

	return ALLOC Framebuffer(buffer_id, width, height, texture_handles);
}

Framebuffer* generate_screen_texture(unsigned int width, unsigned int height)
{
	GLuint screen_texture = 0;
	GLuint screen_RBO_depth = 0;
	GLuint screen_FBO = 0;

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &screen_texture);
	glBindTexture(GL_TEXTURE_2D, screen_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, 
		GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &screen_RBO_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, screen_RBO_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
		width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &screen_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, screen_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		screen_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, screen_RBO_depth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<TextureHandle> textures = { screen_texture, screen_RBO_depth };

	return ALLOC Framebuffer(screen_FBO, width, height, textures);
}

Framebuffer* generate_shadow_buffers()
{
	GLuint buffer_id;
	glGenFramebuffers(1, &buffer_id);

	GLuint texture_ids[SHADOW_MAP_CASCADE_COUNT];

	glGenTextures(SHADOW_MAP_CASCADE_COUNT, texture_ids);

	for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_DEPTH_COMPONENT,
			SHADOW_MAP_WIDTH,
			SHADOW_MAP_HEIGHT,
			0,
			GL_DEPTH_COMPONENT,
			GL_FLOAT,
			nullptr
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, buffer_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		texture_ids[0], 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("Failed to create cascade shadow map framebuffer");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<TextureHandle> texture_handles;
	for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
	{
		texture_handles.push_back(static_cast<TextureHandle>(texture_ids[i]));
	}

	return ALLOC Framebuffer(buffer_id, SHADOW_MAP_WIDTH,
		SHADOW_MAP_HEIGHT, texture_handles);
}

PipelineManager::PipelineManager(const Window& window,
	DeletionQueue* const deletion_queue, ShaderMap& shaders)
	: data{ std::make_unique<Data>(window) }
	, deletion_queue{ deletion_queue }
{
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//NOTE(ches) Support for transparencies
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

PipelineManager::~PipelineManager()
{
	delete_resource(data->back_buffer, deletion_queue);
	delete_resource(data->gbuffer, deletion_queue);
	delete_resource(data->screen_texture, deletion_queue);
	delete_resource(data->shadow_buffer, deletion_queue);
	delete_resource(data->point_lights, deletion_queue);
	delete_resource(data->spot_lights, deletion_queue);
}

Pipeline* PipelineManager::get_pipeline(RenderConfig config)
{
	if (data->pipelines.contains(config))
	{
		return data->pipelines.find(config)->second;
	}

	Pipeline* result = build_pipeline(config);

	data->pipelines.insert(std::pair(config, result));

	return result;
}

void PipelineManager::resize(int width, int height)
{
	data->cached_width = width;
	data->cached_height = height;
	delete_resource(data->screen_texture, deletion_queue);
	data->screen_texture = generate_screen_texture(width, height);
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

/// <summary>
/// Set up material IDs.
/// </summary>
/// <param name="scene">The scene we will be rendering.</param>
void recalculate_materials(const Scene& scene)
{
	const bool for_animated_models = true;
	const bool for_static_models = false;

	calculate_materials(scene, for_animated_models);
	calculate_materials(scene, for_static_models);
}

void setup_animated_command_buffer(RenderBuffers* render_buffers,
	CommandBuffers* command_buffers, Scene& scene)
{
	const std::vector<std::shared_ptr<Model>>& model_list =
		scene.get_animated_model_list();

	render_buffers->load_animated_entity_buffers(scene);

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
		command_buffers->animated_model_matrices_buffer);
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

	command_buffers->animated_draw_count = static_cast<unsigned int>(mesh_count);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
		command_buffers->animated_command_buffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, data_size_in_bytes, command_buffer,
		GL_STATIC_DRAW);
	safe_delete_array(command_buffer);

	data_size_in_bytes = mesh_count * DRAW_ELEMENT_SIZE * sizeof(int);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers->animated_draw_element_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, draw_elements,
		GL_STATIC_DRAW);
	safe_delete_array(draw_elements);
}

void setup_static_command_buffer(CommandBuffers* command_buffers,
	const Scene& scene)
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
		command_buffers->static_model_matrices_buffer);
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

	command_buffers->static_draw_count = static_cast<unsigned int>(mesh_count);

	glBindBuffer(GL_DRAW_INDIRECT_BUFFER,
		command_buffers->static_command_buffer);
	glBufferData(GL_DRAW_INDIRECT_BUFFER, data_size_in_bytes, command_buffer,
		GL_STATIC_DRAW);
	safe_delete_array(command_buffer);

	data_size_in_bytes = draw_element_count * DRAW_ELEMENT_SIZE * sizeof(int);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER,
		command_buffers->static_draw_element_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, data_size_in_bytes, draw_elements,
		GL_STATIC_DRAW);
	safe_delete_array(draw_elements);
}

void refresh_animated_data(RenderBuffers* render_buffers,
	CommandBuffers* command_buffers, Scene& scene)
{
	if (scene.animated_models_dirty)
	{
		render_buffers->load_animated_models(scene);
	}
	if (scene.animated_models_dirty || scene.animated_entities_dirty)
	{
		setup_animated_command_buffer(render_buffers, command_buffers, scene);
	}
	scene.animated_entities_dirty = false;
	scene.animated_models_dirty = false;
}

void refresh_static_data(RenderBuffers* render_buffers,
	CommandBuffers* command_buffers, Scene& scene)
{
	if (scene.static_models_dirty)
	{
		render_buffers->load_static_models(scene);
	}
	if (scene.static_models_dirty || scene.static_entities_dirty)
	{
		setup_static_command_buffer(command_buffers, scene);
	}
	scene.static_entities_dirty = false;
	scene.static_models_dirty = false;
}

void PipelineManager::setup_data(Scene& scene)
{
	TIME_START("Updating Scene - Updating Data - Materials");
	recalculate_materials(scene);
	TIME_END("Updating Scene - Updating Data - Materials");

	TIME_START("Updating Scene - Updating Data - Static");
	if (scene.static_models_dirty || scene.static_entities_dirty)
	{
		refresh_static_data(data->render_buffers, data->command_buffers, scene);
	}
	TIME_END("Updating Scene - Updating Data - Static");

	TIME_START("Updating Scene - Updating Data - Animated");
	if (scene.animated_models_dirty || scene.animated_entities_dirty)
	{
		refresh_animated_data(data->render_buffers, data->command_buffers, 
			scene);
	}
	TIME_END("Updating Scene - Updating Data - Animated");

#if _DEBUG
	TIME_START("Updating Scene - Updating Data - Debug Lines");
	if (Instance::configuration.debug_lines)
	{
		data->debug_render.update_lines(scene);
	}
	TIME_END("Updating Scene - Updating Data - Debug Lines");
#endif

	scene.dirty = false;
}

Pipeline* PipelineManager::build_pipeline(RenderConfig config)
{
	Pipeline* result = ALLOC Pipeline();

	bool rendering_scene = false;

	if (config & RenderConfigValues::ANIMATION_PASS_MASK
		|| config & RenderConfigValues::SHADOW_PASS_MASK
		|| config & RenderConfigValues::SCENE_PASS_MASK
		|| config & RenderConfigValues::LIGHTING_PASS_MASK
		|| config & RenderConfigValues::SKYBOX_PASS_MASK
		|| config & RenderConfigValues::FILTER_PASS_MASK
		)
	{
		rendering_scene = true;
	}

	if (rendering_scene)
	{
		result->render_stages.push_back(&(data->model_matrix_update));
	}

	if (config & RenderConfigValues::ANIMATION_PASS_MASK)
	{
		result->render_stages.push_back(&(data->animation_render));
	}

	if (config & RenderConfigValues::SHADOW_PASS_MASK)
	{
		result->render_stages.push_back(&(data->shadow_render));
	}

	if (config & RenderConfigValues::SCENE_PASS_MASK)
	{
		if (config & RenderConfigValues::SCENE_WIREFRAME_MASK)
		{
			result->render_stages.push_back(&(data->scene_render_wireframe));
		}
		else
		{
			result->render_stages.push_back(&(data->scene_render));
		}
	}

	if (config & RenderConfigValues::FILTER_PASS_MASK)
	{
		result->render_stages.push_back(&(data->screen_texture_binding));
	}
	else
	{
		result->render_stages.push_back(&(data->back_buffer_binding));
	}

	if (config & RenderConfigValues::LIGHTING_PASS_MASK)
	{
		result->render_stages.push_back(&(data->light_render));
	}

	if (config & RenderConfigValues::SKYBOX_PASS_MASK)
	{
		result->render_stages.push_back(&(data->skybox_render));
	}

	if (config & RenderConfigValues::FILTER_PASS_MASK)
	{
		result->render_stages.push_back(&(data->back_buffer_binding));
		result->render_stages.push_back(&(data->filter_render));
	}

#if _DEBUG
	if (config & RenderConfigValues::DEBUG_PASS_MASK)
	{
		result->render_stages.push_back(&(data->debug_render));
	}
#endif

	if (config & RenderConfigValues::GUI_PASS_MASK)
	{
		if (rendering_scene)
		{
			result->render_stages.push_back(&(data->gui_render));
		}
		else
		{
			result->render_stages.push_back(&(data->gui_render_standalone));
		}
	}

	return result;
}

#endif
