#include "Fog.h"

Fog::Fog()
	: active(false)
	, color()
	, density(0.0f)
{}

Fog::Fog(const bool active, const glm::vec3& color, const float density)
	: active(active)
	, color(color)
	, density(density)
{}

void Fog::set_color(const float r, const float g, const float b)
{
	color.r = r;
	color.g = g;
	color.b = b;
}