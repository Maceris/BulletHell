#include "Texture.h"

std::shared_ptr<Texture> Texture::default_texture = nullptr;

Texture::Texture(const unsigned int width, const unsigned int height,
	const char* buffer)
	: width(width)
	, height(height)
	, texture_id(0)
{
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	// byte aligned
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, 
		GL_UNSIGNED_BYTE, buffer);
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture()
{
	glDeleteTextures(1, &texture_id);
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
}