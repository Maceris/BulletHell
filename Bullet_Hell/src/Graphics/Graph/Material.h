#pragma once

#include "Globals.h"

#include "glm.hpp"

#include <memory>

#include "Texture.h"

/// <summary>
/// Used to identify materials in the material cache.
/// </summary>
using material_id = int;

/// <summary>
/// The default color for materials.
/// </summary>
constexpr glm::vec4 MATERIAL_DEFAULT_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

/// <summary>
/// A material that describes details about a mesh, like color.
/// </summary>
struct Material
{
	/// <summary>
	/// The ambient color of the material.
	/// </summary>
	glm::vec4 ambient_color;

	/// <summary>
	/// The diffucse color of the material.
	/// </summary>
	glm::vec4 diffuse_color;

	/// <summary>
	/// The specular color of the material.
	/// </summary>
	glm::vec4 specular_color;
	
	/// <summary>
	/// The reflectance value of the material.
	/// </summary>
	float reflectance;

	/// <summary>
	/// The texture for this material.
	/// </summary>
	std::string texture_name;
	
	/// <summary>
	/// The normal map texture for this material.
	/// </summary>
	std::string normal_map_name;

	/// <summary>
	/// Where this material is referenced in the materials uniform.
	/// </summary>
	material_id material_id;

	/// <summary>
	/// Create a material with all default values.
	/// </summary>
	Material()
		: ambient_color(MATERIAL_DEFAULT_COLOR)
		, diffuse_color(MATERIAL_DEFAULT_COLOR)
		, specular_color(MATERIAL_DEFAULT_COLOR)
		, reflectance(0.0f)
		, texture_name("")
		, normal_map_name("")
		, material_id(0)
	{}

	/// <summary>
	/// Copy a material.
	/// </summary>
	Material(const Material&) = default;

	/// <summary>
	/// Copy a material.
	/// </summary>
	Material& operator=(const Material&) = default;

	/// <summary>
	/// Clean up a material.
	/// </summary>
	~Material() = default;
};

