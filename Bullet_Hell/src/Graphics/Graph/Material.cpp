#include "Material.h"

Material::Material()
	: ambient_color(MATERIAL_DEFAULT_COLOR)
	, diffuse_color(MATERIAL_DEFAULT_COLOR)
	, specular_color(MATERIAL_DEFAULT_COLOR)
	, reflectance(0.0f)
	, texture(0)
	, normal_map(0)
	, material_id(0)
{}
