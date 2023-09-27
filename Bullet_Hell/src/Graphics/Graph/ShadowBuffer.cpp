#include "ShadowBuffer.h"

#include "CascadeShadowSlice.h"

ShadowBuffer::ShadowBuffer()
	: depth_map(CascadeShadowSlice::SHADOW_MAP_CASCADE_COUNT, SHADOW_MAP_WIDTH,
		SHADOW_MAP_HEIGHT, GL_DEPTH_COMPONENT)
	, depth_map_fbo(0)
{
	glGenFramebuffers(1, &depth_map_fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		depth_map[0], 0);

	// We only want depth
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_WARNING("Failed to create shadow buffer");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowBuffer::~ShadowBuffer()
{
	glDeleteFramebuffers(1, &depth_map_fbo);
	// depth map destructor cleans itself up
}

void ShadowBuffer::bind_textures(int start)
{
	for (int i = 0; i < CascadeShadowSlice::SHADOW_MAP_CASCADE_COUNT; ++i)
	{
		glActiveTexture(start + i);
		glBindTexture(GL_TEXTURE_2D, depth_map[i]);
	}
}