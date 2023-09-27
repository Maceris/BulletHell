#pragma once

#include "Globals.h"

#include "ArrayOfTextures.h"

#include "glad.h"

/// <summary>
/// Depth buffers for a cascaded shadow map.
/// </summary>
class ShadowBuffer
{
public:
	/// <summary>
	/// The width of the shadow map in pixels.
	/// </summary>
	static const unsigned int SHADOW_MAP_WIDTH = 4096;

	/// <summary>
	/// The height of the shadow map in pixels.
	/// </summary>
	static const unsigned int SHADOW_MAP_HEIGHT = 4096;

	/// <summary>
	/// The actual depth map textures used for the FBO.
	/// </summary>
	ArrayOfTextures depth_map;

	/// <summary>
	/// The frame buffer object for our shadow buffer.
	/// </summary>
	GLuint depth_map_fbo;

	/// <summary>
	/// Set up a shadow buffer.
	/// </summary>
	ShadowBuffer();
	
	/// <summary>
	/// Don't allow copy.
	/// </summary>
	ShadowBuffer(const ShadowBuffer&) = delete;
	
	/// <summary>
	/// Don't allow copy.
	/// </summary>
	ShadowBuffer& operator=(const ShadowBuffer&) = delete;

	/// <summary>
	/// Clean up the buffers.
	/// </summary>
	~ShadowBuffer();

	/// <summary>
	/// Bind the texures in the shadow buffer, starting at the specified index.
	/// The index should be something along the lines of "GL_TEXTURE0 + n", 
	/// so that we can bind these after some number of other textures.
	/// </summary>
	/// <param name="start">The index to start with for glBindTexture.</param>
	void bind_textures(int start);
};