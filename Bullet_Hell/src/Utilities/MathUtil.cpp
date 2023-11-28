#include "Utilities/MathUtil.h"

#include "glm.hpp"

/// <summary>
/// Take an angle in degrees as given by glm in the range (-180, 180],
/// and convert it to [0, 360) starting at +x and going clockwise.
/// </summary>
/// <param name="degrees">The angle in degrees.</param>
/// <returns>The degrees in a format that is nicer for use.</returns>
float normalize_float_angle(const float& degrees)
{
	if (MathUtil::close_enough(degrees, 0)
		|| MathUtil::close_enough(degrees, 180))
	{
		return degrees;
	}
	if (degrees < 0)
	{
		return -degrees;
	}
	return 360.0f - degrees;
}

float MathUtil::clamp_float(const float& value, const float& min,
	const float& max)
{
	if (std::isnan(value)) {
		return min;
	}
	return std::clamp<float>(value, min, max);
}

bool MathUtil::close_enough(const float& a, const float& b)
{
	return !std::isnan(a) && !std::isnan(b) && std::abs(a - b) < 0.0001f;
}

bool MathUtil::close_enough(const glm::vec2& vector, const float& y,
	const float& x)
{
	return MathUtil::close_enough(vector.x, x) 
		&& MathUtil::close_enough(vector.y, y);
}

float MathUtil::vector_to_angle(const glm::vec2& direction)
{
	//NOTE(ches) atan is in radians (+/-) with respect to the +x axis
	float angle = static_cast<float>(glm::degrees(
		glm::atan(direction.x, direction.y)
	));
	return normalize_float_angle(angle);
}