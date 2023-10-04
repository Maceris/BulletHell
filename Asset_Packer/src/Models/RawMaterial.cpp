#include "RawMaterial.h"

RawMaterial::RawMaterial()
	: ambient_color(RAW_MATERIAL_DEFAULT_COLOR)
	, diffuse_color(RAW_MATERIAL_DEFAULT_COLOR)
	, specular_color(RAW_MATERIAL_DEFAULT_COLOR)
	, reflectance(0.0f)
	, texture()
	, normal_map()
{}
