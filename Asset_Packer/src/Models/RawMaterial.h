#pragma once

#include "glm.hpp"

#include <string>

constexpr glm::vec4 RAW_MATERIAL_DEFAULT_COLOR = 
	glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

/// <summary>
/// A material that describes details about a mesh, like color, loaded from
/// a raw mesh.
/// </summary>
struct RawMaterial
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
	/// The name of the texture for this material.
	/// </summary>
	std::string texture;
	
	/// <summary>
	/// The name of the normal map texture for this material.
	/// </summary>
	std::string normal_map;

	/// <summary>
	/// Create a material with all default values.
	/// </summary>
	RawMaterial();

	/// <summary>
	/// Copy a material.
	/// </summary>
	RawMaterial(const RawMaterial&) = default;

	/// <summary>
	/// Copy a material.
	/// </summary>
	RawMaterial& operator=(const RawMaterial&) = default;

	/// <summary>
	/// Clean up a material.
	/// </summary>
	~RawMaterial() = default;
};

