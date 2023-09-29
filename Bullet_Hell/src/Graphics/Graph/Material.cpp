#include "Material.h"

const glm::vec4 MATERIAL_DEFAULT_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

Material::Material()
	: ambient_color(MATERIAL_DEFAULT_COLOR)
	, diffuse_color(MATERIAL_DEFAULT_COLOR)
	, specular_color(MATERIAL_DEFAULT_COLOR)
	, reflectance(0.0f)
	, texture(0)
	, normal_map(0)
{}
