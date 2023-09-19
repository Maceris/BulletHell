#pragma once

#include "Globals.h"

#include "glm.hpp"

#include "Texture.h"

struct Material
{
	/// <summary>
	/// The default color for materials.
	/// </summary>
	static const glm::vec4 DEFAULT_COLOR;

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
	/// A mesh uses only a single material, so if an imported model uses
	/// multiple materials, the import splits up the mesh. This value is used
	/// as an index into the scene's material list.
	/// </summary>
	int material_index;

	/// <summary>
	/// The ID of the texture for this material.
	/// </summary>
	texture_id texture;
	
	/// <summary>
	/// The ID of the normal map texture for this material.
	/// </summary>
	texture_id normal_map;

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

