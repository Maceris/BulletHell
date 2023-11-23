#pragma once

#include <algorithm>
#include <cmath>

#include "vec2.hpp"

namespace MathUtil
{
	/// <summary>
	/// Clamps a float between given values. NaN is considered less than 
	/// the minimum.
	/// Anything less than min will be increased to min, anything greater 
	/// than max will be reduced to max, and anything in between will
	/// be left alone.
	/// </summary>
	/// <param name="value">The value we are clamping.</param>
	/// <param name="min">The smallest value to allow, inclusive.</param>
	/// <param name="max">The largest value to allow, inclusive.</param>
	/// <returns>The value restricted to being between min and max.</returns>
	float clamp_float(const float& value, const float& min, const float& max);

	/// <summary>
	/// Check if two floats are within a reasonable distance to each other. If 
	/// either is NaN, they are not considered to be the close enough.
	/// </summary>
	/// <param name="a">The first float.</param>
	/// <param name="b">The second float.</param>
	/// <returns>If the floats are close enough to equals for our purposes.
	/// </returns>
	bool close_enough(const float& a, const float& b);

	/// <summary>
	/// Checks if a vector is close enough to the specified float values.
	/// If any of the coordinates are NaN, they are not considered to be the
	/// close enough.
	/// </summary>
	/// <param name="vector">The vector to check.</param>
	/// <param name="y">The value to check the vectors first value against.
	/// </param>
	/// <param name="x">The value to check the vectors second value against.
	/// </param>
	/// <returns>If the vectors values are both close enough to the given 
	/// values for our purposes.</returns>
	bool close_enough(const glm::vec2& vector, const float& y,
		const float& x);
};
