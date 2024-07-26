#pragma once

#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/render_stage.h"

class FramebufferTransition : RenderStage
{
public:
	FramebufferTransition(StageResource<Framebuffer>* framebuffer,
		uint32_t blend_s_factor, uint32_t blend_d_factor)
		: framebuffer{ framebuffer }
		, blend_s_factor{ blend_s_factor }
		, blend_d_factor{ blend_d_factor }
	{}

	void render(Scene& scene);

private:
	StageResource<Framebuffer>* const framebuffer;
	uint32_t const blend_s_factor;
	uint32_t const blend_d_factor;
};