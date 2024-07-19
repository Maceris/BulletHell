#pragma once

#include "globals.h"

#include "glm/vec3.hpp"

/// <summary>
/// Represents the fog in a scene.
/// </summary>
struct Fog
{
	/// <summary>
	/// Whether fog is active.
	/// </summary>
	bool active;

	/// <summary>
	/// The base color of the fog. Values should all be between 0 and 1,
	/// inclusive.
	/// </summary>
	glm::vec3 color;

	/// <summary>
	/// How thick the fog is, modeled as 1/(e^(distance*density)).
	/// </summary>
	float density;

	/// <summary>
	/// Sets up fog as disabled.
	/// </summary>
	Fog();

	/// <summary>
	/// Set up fog.
	/// </summary>
	/// <param name="active">Whether fog is active.</param>
	/// <param name="color">The color of the fog.</param>
	/// <param name="density">Density of the fog, modeled as 
	/// 1/(e^(distance*density)).</param>
	Fog(const bool active, const glm::vec3& color, const float density);

	Fog(const Fog&) = default;
	Fog& operator=(const Fog&) = default;
	~Fog() = default;

	/// <summary>
	/// Set the color of the fog. Values should all be between 0 
	/// and 1, inclusive.
	/// </summary>
	/// <param name="r">The red component.</param>
	/// <param name="g">The green component.</param>
	/// <param name="b">The blue component.</param>
	void set_color(const float r, const float g, const float b);
};