#pragma once

#include "Globals.h"

#include "glm.hpp"

/// <summary>
/// A symmetric perspective projection frustum transformation for a
/// right - handed coordinate system using OpenGL's NDC z range of [-1.. + 1],
/// and its inverse.
/// </summary>
class Projection
{
public:
	
	/// <summary>
	/// The inverse of the projection matrix.
	/// </summary>
	glm::mat4 inverse_projection_matrix;

	/// <summary>
	/// The current projection matrix.
	/// </summary>
	glm::mat4 projection_matrix;
	
	/// <summary>
	/// Create a new projection matrix, and calculate it's inverse.
	/// </summary>
	/// <param name="width">The width of the window, in pixels.</param>
	/// <param name="height">The height of the window, in pixels.</param>
	Projection(const unsigned int width, const unsigned int height);

	/// <summary>
	/// Update the projection (and inverse) to a new window size.
	/// </summary>
	/// <param name="width">The width of the window, in pixels.</param>
	/// <param name="height">The height of the window, in pixels.</param>
	void update_matrices(const unsigned int width, const unsigned int height);

private:
	/// <summary>
	/// The far clipping plane distance, in world units.
	/// </summary>
	const float Z_FAR = 1000.0f;

	/// <summary>
	/// The near clipping plane distance, in world units.
	/// </summary>
	const float Z_NEAR = 0.01f;

	/// <summary>
	/// The field of view in radians.
	/// </summary>
	const float FOV = glm::radians(60.0f);
};