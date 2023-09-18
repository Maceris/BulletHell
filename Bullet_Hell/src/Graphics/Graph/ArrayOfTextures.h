#pragma once

#include "Globals.h"

#include "glad.h"

/// <summary>
/// An array of textures, all of the same size and pixel format.
/// </summary>
struct ArrayOfTextures
{

	/// <summary>
	/// The array of textures that has been allocated.
	/// </summary>
	GLuint* textures;

	/// <summary>
	/// The number of textures in the array.
	/// </summary>
	const unsigned int texture_count;

	/// <summary>
	/// Create an array of textures.
	/// </summary>
	/// <param name="texture_count">The number of textures to create.</param>
	/// <param name="width">The width of each texture in pixels.</param>
	/// <param name="height">The height of each texture in pixels.</param>
	/// <param name="pixel_format">The pixel format to use for the textures for
	/// glTexImage2Ds.</param>
	ArrayOfTextures(unsigned int texture_count, unsigned int width,
		unsigned int height, int pixel_format)
		: texture_count(texture_count), textures(new GLuint[texture_count])
	{
		glGenTextures(texture_count, textures);
		for (int i = 0; i < texture_count; ++i) {
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
				0, pixel_format, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
				GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
				GL_CLAMP_TO_EDGE);
		}
	}

	/// <summary>
	/// Texture arrays cannot be copied.
	/// </summary>
	ArrayOfTextures(const ArrayOfTextures&) = delete;
	/// <summary>
	/// Texture arrays cannot be copied.
	/// </summary>
	ArrayOfTextures& operator=(const ArrayOfTextures&) = delete;

	/// <summary>
	/// Delete the textures and clean up memory.
	/// </summary>
	~ArrayOfTextures()
	{
		glDeleteTextures(texture_count, textures);
		delete[] textures;
	}
};