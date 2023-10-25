#include "GBuffer.h"

GBuffer::GBuffer(const unsigned int width, const unsigned int height)
	: width(width)
	, height(height)
	, gBuffer_ID(0)
	, texture_IDs()
{
	glGenFramebuffers(1, &gBuffer_ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer_ID);
	glGenTextures(G_BUFFER_TOTAL_TEXTURES, texture_IDs);

	for (int i = 0; i < G_BUFFER_TOTAL_TEXTURES; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, texture_IDs[i]);
		int attachment_type;
		if (i == G_BUFFER_TOTAL_TEXTURES - 1)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width,
				height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			attachment_type = GL_DEPTH_ATTACHMENT;
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0,
				GL_RGBA, GL_FLOAT, nullptr);
			attachment_type = GL_COLOR_ATTACHMENT0 + i;
		}
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, GL_TEXTURE_2D,
			texture_IDs[i], 0);
	}

	GLenum draw_buffers[G_BUFFER_TOTAL_TEXTURES - 1]{};
	for (int i = 0; i < G_BUFFER_TOTAL_TEXTURES - 1; ++i)
	{
		draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	glDrawBuffers(G_BUFFER_TOTAL_TEXTURES - 1, draw_buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GBuffer::~GBuffer()
{
	glDeleteFramebuffers(1, &gBuffer_ID);
	glDeleteTextures(G_BUFFER_TOTAL_TEXTURES, texture_IDs);
}