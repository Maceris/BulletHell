#pragma once

#include <algorithm>
#include <cmath>

class MathUtil
{
public:
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
	static float inline clamp_float(const float value, const float min,
		const float max)
	{
		if (std::isnan(value)) {
			return min;
		}
		return std::clamp<float>(value, min, max);
	}
};
