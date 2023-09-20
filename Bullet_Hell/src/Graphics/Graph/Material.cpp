#include "Material.h"

const glm::vec4 Material::DEFAULT_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

Material::Material()
	: ambient_color(DEFAULT_COLOR)
	, diffuse_color(DEFAULT_COLOR)
	, specular_color(DEFAULT_COLOR)
	, reflectance(0.0f)
	, texture(0)
	, normal_map(0)
{}
