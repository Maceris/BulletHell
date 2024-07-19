#include "graphics/scene/lights/ambient_light.h"

AmbientLight::AmbientLight()
	: color{ 1.0f, 1.0f, 1.0f }
	, intensity{ 1.0f }
{}

AmbientLight::AmbientLight(const glm::vec3& color, const float intensity)
	: color{ color }
	, intensity{ intensity }
{}

void AmbientLight::set_color(const float r, const float g, const float b)
{
	color.r = r;
	color.g = g;
	color.b = b;
}
