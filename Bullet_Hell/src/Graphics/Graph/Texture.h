#pragma once

#include "Globals.h"

#include <string>
#include <memory>

#include "glad.h"

class Texture
{
public:
	/// <summary>
	/// The OpenGL texture ID.
	/// </summary>
	GLuint texture_id;

	/// <summary>
	/// The width of the texture in pixels.
	/// </summary>
	const unsigned int width;

	/// <summary>
	/// The height of the texture in pixels.
	/// </summary>
	const unsigned int height;

	/// <summary>
	/// Load a texture from a buffer.
	/// </summary>
	/// <param name="width">The width of the texture in pixels.</param>
	/// <param name="height">The height of the texture in pixels.</param>
	/// <param name="buffer">The texture data to load from.</param>
	Texture(const unsigned int width, const unsigned int height, 
		const char* buffer);

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;
	~Texture();

	/// <summary>
	/// Bind the texture for rendering.
	/// </summary>
	void bind();

	/// <summary>
	/// The default texture to use, which is loaded during initialization.
	/// </summary>
	static std::shared_ptr<Texture> default_texture;
};