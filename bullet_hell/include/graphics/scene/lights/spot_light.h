#pragma once

#include "glm/vec3.hpp"

#include "graphics/scene/lights/point_light.h"

/// <summary>
/// A light that shines in a cone.
/// </summary>
struct SpotLight
{
	/// <summary>
	/// The direction the light is pointing towards.
	/// </summary>
	glm::vec3 cone_direction;

	/// <summary>
	/// The angle where we cut the light off, stored as cos(cut-off angle in 
	/// radians).
	/// </summary>
	float cut_off;

	/// <summary>
	/// The light we are using, which is cut off everywhere except the cone.
	/// </summary>
	PointLight point_light;

	/// <summary>
	/// Set up a spot light.
	/// </summary>
	/// <param name="point_light">The point light that describes the lighting
	/// properties.</param>
	/// <param name="cone_direction">The direction the light is facing.</param>
	/// <param name="cut_off_degrees">The angle in degrees away from the
	/// directionthat we cut off lighte light at.</param>
	SpotLight(const PointLight& point_light, const glm::vec3& cone_direction,
		float cut_off_degrees);

	/// <summary>
	/// Set the direction that the light is shining in. This is the x, y, and
	/// z coordinates of a vector that represent the direction of the light.
	/// </summary>
	/// <param name="r">The x component.</param>
	/// <param name="g">The y component.</param>
	/// <param name="b">The z component.</param>
	void set_cone_direction(const float x, const float y, const float z);

	/// <summary>
	/// Set the cutoff angle, in degrees. Converted to
	/// cos(toRadians(angle in degrees)).
	/// </summary>
	/// <param name="cut_off_degrees">The cut off angle in degrees.</param>
	void set_cut_off_angle(const float cut_off_degrees);
};