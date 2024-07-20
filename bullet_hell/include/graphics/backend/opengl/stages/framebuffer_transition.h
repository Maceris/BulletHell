#pragma once

#include "graphics/backend/base/render_stage.h"
#include "graphics/frontend/framebuffer.h"

class FramebufferTransition : RenderStage
{
public:
	FramebufferTransition(Framebuffer* framebuffer,
		uint32_t blend_s_factor, uint32_t blend_d_factor)
		: framebuffer{ framebuffer }
		, blend_s_factor{ blend_s_factor }
		, blend_d_factor{ blend_d_factor }
	{}

	void render(Scene& scene);

private:
	const Framebuffer* framebuffer;
	const uint32_t blend_s_factor;
	const uint32_t blend_d_factor;
};