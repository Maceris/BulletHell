#include "PointLight.h"

PointLight::PointLight(const glm::vec3& position)
	: attenuation()
	, color(1.0f, 1.0f, 1.0f)
	, position(position)
	, intensity(1.0f)
{}

PointLight::PointLight(const glm::vec3& color, const glm::vec3& position,
	const float intensity)
	: attenuation()
	, color(color)
	, position(position)
	, intensity(intensity)
{}

PointLight::PointLight(const glm::vec3& color, const glm::vec3& position,
const float intensity, const Attenuation& attenuation)
	: attenuation(attenuation)
	, color(color)
	, position(position)
	, intensity(intensity)
{}

void PointLight::set_color(const float r, const float g, const float b)
{
	color.r = r;
	color.g = g;
	color.b = b;
}

void PointLight::set_position(const float x, const float y, const float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void PointLight::set_attenuation(const float constant, const float linear,
	const float exponent)
{
	attenuation.constant = constant;
	attenuation.linear = linear;
	attenuation.exponent = exponent;
}
