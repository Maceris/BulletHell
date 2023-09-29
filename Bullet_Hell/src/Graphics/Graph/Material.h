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
extern const glm::vec4 MATERIAL_DEFAULT_COLOR;

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
	std::shared_ptr<Texture> texture;
	
	/// <summary>
	/// The normal map texture for this material.
	/// </summary>
	std::shared_ptr<Texture> normal_map;

	/// <summary>
	/// Create a material with all default values.
	/// </summary>
	Material();

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

