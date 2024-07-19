#pragma once

#include "globals.h"

#include "glm/vec3.hpp"

/// <summary>
/// A point light in the scene.
/// </summary>
struct PointLight
{
	/// <summary>
	/// Attenuation info for a light. Controls the way light falls off with
	/// distance. Brightness is calculated using this formula:
	/// 
	/// 1.0/(constant + linear * distance + exponent * distance^2)
	/// </summary>
	struct Attenuation
	{
		/// <summary>
		/// The constant component for fall off.
		/// </summary>
		float constant;

		/// <summary>
		/// The linear fall off with respect to distance.
		/// </summary>
		float linear;

		/// <summary>
		/// The exponential fall off with respect to distance.
		/// </summary>
		float exponent;
		
		/// <summary>
		/// Set up linear attenuation.
		/// </summary>
		Attenuation()
			: constant(0.0f)
			, linear(1.0f)
			, exponent(0.0f) {}

		/// <summary>
		/// Set up specific attenuation.
		/// </summary>
		/// <param name="constant">The constant component.</param>
		/// <param name="linear">The linear component.</param>
		/// <param name="exponent">The exponent component.</param>
		Attenuation(const float constant, const float linear,
			const float exponent)
			: constant(constant)
			, linear(linear)
			, exponent(exponent) {}

		Attenuation(const Attenuation&) = default;
	};

	/// <summary>
	/// Information about how the light is attenuated by distance.
	/// </summary>
	Attenuation attenuation;

	/// <summary>
	/// The base color of the light. Values should all be between 0 and 1,
	/// inclusive.
	/// </summary>
	glm::vec3 color;

	/// <summary>
	/// The position of the light, in world space.
	/// </summary>
	glm::vec3 position;

	/// <summary>
	/// How bright the light is. Should be between 0 and 1, inclusive.
	/// </summary>
	float intensity;

	/// <summary>
	/// Create a point light at the specified position, with default values 
	/// and constant attenuation.
	/// </summary>
	/// <param name="position">The position of the light.</param>
	PointLight(const glm::vec3& position);

	/// <summary>
	/// Create a point light with a constant attenuation.
	/// </summary>
	/// <param name="color">The color of the light.</param>
	/// <param name="position">The position of the light.</param>
	/// <param name="intensity">The intensity of the light.</param>
	PointLight(const glm::vec3& color, const glm::vec3& position,
		const float intensity);

	/// <summary>
	/// Create a point light.
	/// </summary>
	/// <param name="color">The color of the light.</param>
	/// <param name="position">The position of the light.</param>
	/// <param name="intensity">The intensity of the light.</param>
	/// <param name="attenuation">Attenuation of the light.</param>
	PointLight(const glm::vec3& color, const glm::vec3& position,
		const float intensity, const Attenuation& attenuation);

	/// <summary>
	/// Set the color of the light. Values should all be between 0 
	/// and 1, inclusive.
	/// </summary>
	/// <param name="r">The red component.</param>
	/// <param name="g">The green component.</param>
	/// <param name="b">The blue component.</param>
	void set_color(const float r, const float g, const float b);

	/// <summary>
	/// Set the position of the light, in world coordinates.
	/// </summary>
	/// <param name="r">The x position.</param>
	/// <param name="g">The y position.</param>
	/// <param name="b">The z position.</param>
	void set_position(const float x, const float y, const float z);

	/// <summary>
	/// Set the attenuation of the light. Controls the way light falls off with
	/// distance. Brightness is calculated using this formula:
	/// 
	/// 1.0/(constant + linear * distance + exponent * distance^2)
	/// </summary>
	/// <param name="constant">The constant component.</param>
	/// <param name="linear">The linear component.</param>
	/// <param name="exponent">The exponent component.</param>
	void set_attenuation(const float constant, const float linear,
		const float exponent);
};