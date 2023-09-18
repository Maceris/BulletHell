#pragma once

#include "Globals.h"

#include "glad.h"

/// <summary>
/// A buffer for the deferred shading geometry pass.
/// </summary>
struct GBuffer
{
	/// <summary>
	/// The maximum number of textures, which are used for albedo, normals,
	/// specular, and depth, in that order.
	/// </summary>
	static const unsigned int TOTAL_TEXTURES = 4;

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
	GLuint texture_IDs[TOTAL_TEXTURES];

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
	
	/// <summary>
	/// We cannot copy buffers.
	/// </summary>
	GBuffer(const GBuffer&) = delete;

	/// <summary>
	/// We cannot copy buffers.
	/// </summary>
	GBuffer& operator=(const GBuffer&) = delete;

	/// <summary>
	/// Clean up resources.
	/// </summary>
	~GBuffer();
};