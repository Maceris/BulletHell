#include "graphics/scene/lights/spot_light.h"

#include <cmath>
#include <numbers>

#define degreesToRadians(angleInDegrees) ((angleInDegrees) * std::numbers::pi / 180.0)

SpotLight::SpotLight(const PointLight& point_light, const glm::vec3& cone_direction,
	float cut_off_degrees)
	: point_light{ point_light }
	, cone_direction{ cone_direction }
{
	set_cut_off_angle(cut_off_degrees);
}

void SpotLight::set_cone_direction(const float x, const float y, const float z)
{
	cone_direction.x = x;
	cone_direction.y = y;
	cone_direction.z = z;
}

void SpotLight::set_cut_off_angle(const float cut_off_degrees)
{
	cut_off = static_cast<float>(std::cos(degreesToRadians(cut_off_degrees)));
}