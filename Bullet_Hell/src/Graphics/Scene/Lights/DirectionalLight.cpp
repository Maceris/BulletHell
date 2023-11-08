#include "Graphics/Scene/Lights/DirectionalLight.h"

DirectionalLight::DirectionalLight()
	: color(1.0f, 1.0f, 1.0f)
	, direction(0.0f, 1.1f, 0.0f)
	, intensity(1.0f)
{}

DirectionalLight::DirectionalLight(const glm::vec3& color,
	const glm::vec3& direction, const float intensity)
	: color(color)
	, direction(direction)
	, intensity(intensity)
{}

void DirectionalLight::set_color(const float r, const float g, const float b)
{
	color.r = r;
	color.g = g;
	color.b = b;
}

void DirectionalLight::set_direction(const float x, const float y,
	const float z)
{
	direction.x = x;
	direction.y = y;
	direction.z = z;
}

