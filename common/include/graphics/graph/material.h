#pragma once

#include <memory>
#include <string>

#include "glm/vec4.hpp"

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
	int material_id;

	/// <summary>
	/// Create a material with all default values.
	/// </summary>
	Material()
		: ambient_color{ MATERIAL_DEFAULT_COLOR }
		, diffuse_color{ MATERIAL_DEFAULT_COLOR }
		, specular_color{ MATERIAL_DEFAULT_COLOR }
		, reflectance{ 0.0f }
		, texture_name{ "" }
		, normal_map_name{ "" }
		, material_id{ 0 }
	{}

	Material(const Material&) = default;
	Material& operator=(const Material&) = default;
	~Material() = default;

	bool operator==(const Material&) const noexcept;
	
};

/// <summary>
/// Used to hash materials so we can reuse them.
/// </summary>
struct MaterialHash
{
	std::size_t operator()(const Material&) const noexcept;
};