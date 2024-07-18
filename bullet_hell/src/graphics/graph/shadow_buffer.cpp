#include "Graphics/Graph/ShadowBuffer.h"

#include "Debugging/Logger.h"
#include "Graphics/Graph/CascadeShadowSlice.h"

#include "glad.h"

ShadowBuffer::ShadowBuffer()
	: depth_map{ SHADOW_MAP_CASCADE_COUNT, SHADOW_MAP_WIDTH,
		SHADOW_MAP_HEIGHT, GL_DEPTH_COMPONENT }
	, depth_map_fbo{ 0 }
{
	glGenFramebuffers(1, &depth_map_fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
		depth_map[0], 0);

	//NOTE(ches) We only want depth
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	LOG_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) 
		== GL_FRAMEBUFFER_COMPLETE
		&& "Failed to create shadow buffer");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowBuffer::~ShadowBuffer()
{
	glDeleteFramebuffers(1, &depth_map_fbo);
	//NOTE(ches) depth map destructor cleans itself up
}

void ShadowBuffer::bind_textures(int start)
{
	for (unsigned int i = 0; i < SHADOW_MAP_CASCADE_COUNT; ++i)
	{
		glActiveTexture(start + i);
		glBindTexture(GL_TEXTURE_2D, depth_map[i]);
	}
}