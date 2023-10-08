#pragma once

#include "Globals.h"

#include "glad.h"

/// <summary>
/// The maximum number of textures, which are used for albedo, normals,
/// specular, and depth, in that order.
/// </summary>
#define G_BUFFER_TOTAL_TEXTURES 3

/// <summary>
/// A buffer for the deferred shading geometry pass.
/// </summary>
struct GBuffer
{
	

	/// <summary>
	/// The frame buffer to use for geometry.
	/// </summary>
	GLuint gBuffer_ID;

	/// <summary>
	/// The height of each texture.
	/// </summary>
	const unsigned int height;

	/// <summary>
	/// The list of textures. These are used for albedo, normals,
	/// specular, and depth, in that order.
	/// </summary>
	GLuint texture_IDs[G_BUFFER_TOTAL_TEXTURES];

	/// <summary>
	/// The width of each texture.
	/// </summary>
	const unsigned int width;

	/// <summary>
	/// Create a gBuffer.
	/// </summary>
	/// <param name="width">The width of each texture.</param>
	/// <param name="height">The height of each texture.</param>
	GBuffer(const unsigned int width, const unsigned int height);
	
	GBuffer(const GBuffer&) = delete;
	GBuffer& operator=(const GBuffer&) = delete;
	~GBuffer();
};