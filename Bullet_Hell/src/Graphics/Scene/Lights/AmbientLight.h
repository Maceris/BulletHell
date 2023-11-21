#pragma once

#include "Globals.h"

#include "vec3.hpp"

/// <summary>
/// Ambient lighting for a scene.
/// </summary>
struct AmbientLight
{
	/// <summary>
	/// The base color of the light. Values should all be between 0 and 1,
	/// inclusive.
	/// </summary>
	glm::vec3 color;
	
	/// <summary>
	/// How bright the light is. Should be between 0 and 1, inclusive.
	/// </summary>
	float intensity;

	/// <summary>
	/// Create an ambient light with default values (white, full intensity).
	/// </summary>
	AmbientLight();
	
	/// <summary>
	/// Create an ambient light with specific values.
	/// </summary>
	/// <param name="color">The color of the light. Values should all be
	/// between 0 and 1, inclusive.</param>
	/// <param name="intensity">The intensity of the light, between 0 
	/// and 1 inclusive.</param>
	AmbientLight(const glm::vec3& color, const float intensity);

	/// <summary>
	/// Set the color of the light. Values should all be between 0 
	/// and 1, inclusive.
	/// </summary>
	/// <param name="r">The red component.</param>
	/// <param name="g">The green component.</param>
	/// <param name="b">The blue component.</param>
	void set_color(const float r, const float g, const float b);
};