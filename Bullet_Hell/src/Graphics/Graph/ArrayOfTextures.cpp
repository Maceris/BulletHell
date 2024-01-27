#include "Graphics/Graph/ArrayOfTextures.h"

#include "Debugging/Logger.h"

#include "glad.h"

ArrayOfTextures::ArrayOfTextures(unsigned int texture_count, unsigned int width,
	unsigned int height, int pixel_format)
	: texture_count{ texture_count }
{
	textures = ALLOC GLuint[texture_count];

	glGenTextures(texture_count, textures);
	for (unsigned int i = 0; i < texture_count; ++i) {
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
			0, pixel_format, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			GL_CLAMP_TO_BORDER);
	}
}

GLuint ArrayOfTextures::operator[](const unsigned int index)
{
	LOG_ASSERT(index >= 0 && index < texture_count);

	return textures[index];
}

ArrayOfTextures::~ArrayOfTextures()
{
	glDeleteTextures(texture_count, textures);
	SAFE_DELETE_ARRAY(textures);
}
