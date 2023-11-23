#include "Utilities/MathUtil.h"

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
