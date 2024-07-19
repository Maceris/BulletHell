#pragma once

#include "globals.h"

#include "graphics/glad_types.h"

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
		unsigned int height, int pixel_format);

	ArrayOfTextures(const ArrayOfTextures&) = delete;
	ArrayOfTextures& operator=(const ArrayOfTextures&) = delete;

	/// <summary>
	/// Index directly into the textures.
	/// </summary>
	/// <param name="index">The texture of the index.</param>
	/// <returns>The texture at the supplied index.</returns>
	GLuint operator[](const unsigned int index);

	/// <summary>
	/// Delete the textures and clean up memory.
	/// </summary>
	~ArrayOfTextures();
};