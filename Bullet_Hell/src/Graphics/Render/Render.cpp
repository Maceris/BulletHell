#include "../../Utilities/Globals.h"
#include "Render.h"

Render::Render(const Window& window)
	: gBuffer(GBuffer(window.width, window.height))
	, animation_render(AnimationRender())
	, gui_render(GuiRender(window))
	, light_render(LightRender())
	, scene_render(SceneRender())
	, shadow_render(ShadowRender())
	, sky_box_render(SkyBoxRender())
	, filter_render(FilterRender())
	, render_buffers(RenderBuffers())
	, command_buffer(CommandBuffer())
	, buffers_populated(false)
	, screen_FBO(0)
	, screen_RBO_depth(0)
	, screen_texture(0)
{
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Support for transparencies
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
	//TODO(ches) complete this
}

void Render::render(const Window& window, const Scene& scene)
{
	//TODO(ches) complete this
	update_model_matrices(scene);
}

void Render::resize(const unsigned int width, const unsigned int height)
{
	//TODO(ches) complete this
}

void Render::setup_data(const Scene& scene)
{
	//TODO(ches) complete this
}

void Render::setup_animated_command_buffer(const Scene& scene)
{
	//TODO(ches) complete this
}

void Render::setup_static_command_buffer(const Scene& scene)
{
	//TODO(ches) complete this
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