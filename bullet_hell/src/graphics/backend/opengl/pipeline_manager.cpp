#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/backend/base/pipeline_manager.h"

Texture* PipelineManager::default_texture = nullptr;

#endif

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/base/pipeline_manager.h"

#include <map>

#include "debugging/logger.h"
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
#include "memory/memory_util.h"

#include "glad.h"

Texture* PipelineManager::default_texture = nullptr;

//TODO(ches) BH-51 - fill this out

struct PipelineManager::Data
{
	Data(Window& window);
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
	StageResource<Texture> font;

	int cached_height;
	int cached_width;

	AnimationRender animation_render;
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

PipelineManager::Data::Data(Window& window)
	: point_lights{ ALLOC Buffer(Buffer::Type::SHADER_STORAGE) }
	, spot_lights{ ALLOC Buffer(Buffer::Type::SHADER_STORAGE) }
	, cascade_shadows{}
	, command_buffers{ ALLOC CommandBuffers() }
	, gbuffer{ nullptr }
	, back_buffer{ nullptr }
	, screen_texture{ nullptr }
	, shadow_buffer{ nullptr }
	, gui_mesh{ ALLOC GuiMesh() }
	, quad_mesh{ ALLOC QuadMesh() }
	, render_buffers{ ALLOC RenderBuffers() }
	, skybox{ ALLOC SkyBox() }
	, font{ nullptr }
	, cached_width{0}
	, cached_height{0}
	, animation_render{ &render_buffers }
	, back_buffer_binding{ &back_buffer, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA }
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
{
	//TODO(ches) generate buffers, font
}

PipelineManager::Data::~Data()
{

}

void delete_render_buffers(PipelineManager::Data& data)
{
	
}

/// <summary>
/// Generates new render buffers. They should be deleted first if already
/// present.
/// </summary>
/// <param name="data">The pipeline manager data to update.</param>
void generate_render_buffers(PipelineManager::Data& data)
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.cached_width,
		data.cached_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &screen_RBO_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, screen_RBO_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
		data.cached_width, data.cached_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &screen_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, screen_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		screen_texture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_RENDERBUFFER, screen_RBO_depth);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<TextureHandle> textures = { screen_texture, screen_RBO_depth };

	data.screen_texture = ALLOC Framebuffer(screen_FBO, data.cached_width,
		data.cached_height, textures);
}

PipelineManager::PipelineManager(Window& window, ShaderMap& shaders)
	: data{}
{
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//NOTE(ches) Support for transparencies
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	generate_render_buffers(*data);
}

PipelineManager::~PipelineManager()
{

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

}

void PipelineManager::setup_data(const Scene& scene)
{

}

Pipeline* PipelineManager::build_pipeline(RenderConfig config)
{
	Pipeline* result = ALLOC Pipeline();

	bool rendering_scene = false;

	if (config & RenderConfigValues::ANIMATION_PASS_MASK)
	{
		result->render_stages.push_back(&(data->animation_render));
		rendering_scene = true;
	}

	if (config & RenderConfigValues::SHADOW_PASS_MASK)
	{
		result->render_stages.push_back(&(data->shadow_render));
		rendering_scene = true;
	}

	if (config & RenderConfigValues::SCENE_PASS_MASK)
	{
		if (config & RenderConfigValues::SCENE_WIREFRAME_MASK)
		{
			result->render_stages.push_back(&(data->scene_render));
		}
		else
		{
			result->render_stages.push_back(&(data->scene_render_wireframe));
		}
		rendering_scene = true;
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
		rendering_scene = true;
	}

	if (config & RenderConfigValues::SKYBOX_PASS_MASK)
	{
		result->render_stages.push_back(&(data->skybox_render));
		rendering_scene = true;
	}

	if (config & RenderConfigValues::FILTER_PASS_MASK)
	{
		result->render_stages.push_back(&(data->back_buffer_binding));
		result->render_stages.push_back(&(data->filter_render));
		rendering_scene = true;
	}

	if (rendering_scene)
	{
		result->render_stages.push_back(&(data->model_matrix_update));
	}

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
		rendering_scene = true;
	}

	return result;
}

#endif
