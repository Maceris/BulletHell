#pragma once

#include "Globals.h"

#include "glm.hpp"

/// <summary>
/// A directional light, infinitely far away with no attenuation.
/// </summary>
struct DirectionalLight
{
	/// <summary>
	/// The base color of the light. Values should all be between 0 and 1,
	/// inclusive.
	/// </summary>
	glm::vec3 color;

	/// <summary>
	/// The vector that represents the direction the light is shining towards.
	/// </summary>
	glm::vec3 direction;

	/// <summary>
	/// How bright the light is. Should be between 0 and 1, inclusive.
	/// </summary>
	float intensity;

	/// <summary>
	/// Set up a directional light with default values.
	/// </summary>
	DirectionalLight();

	/// <summary>
	/// Set up a directional light with the specified values.
	/// </summary>
	/// <param name="color">The color of the light.</param>
	/// <param name="direction">The direction of the light.</param>
	/// <param name="intensity">The intensity of the light.</param>
	DirectionalLight(const glm::vec3& color, const glm::vec3& direction,
		const float intensity);

	/// <summary>
	/// Set the color of the light. Values should all be between 0 
	/// and 1, inclusive.
	/// </summary>
	/// <param name="r">The red component.</param>
	/// <param name="g">The green component.</param>
	/// <param name="b">The blue component.</param>
	void set_color(const float r, const float g, const float b);

	/// <summary>
	/// Set the direction that the light is shining in. This is the x, y, and
	/// z coordinates of a vector that represent the direction of the light.
	/// </summary>
	/// <param name="r">The x component.</param>
	/// <param name="g">The y component.</param>
	/// <param name="b">The z component.</param>
	void set_direction(const float x, const float y, const float z);
};