#pragma once

#include "Globals.h"

#include "Material.h"

#include <vector>
#include <map>
#include <string>

/// <summary>
/// A cache of materials so that we can reuse them.
/// </summary>
class MaterialCache
{
public:
	/// <summary>
	/// Set up a new cache with a default material.
	/// </summary>
	MaterialCache();

	/// <summary>
	/// There is only one cache.
	/// </summary>
	MaterialCache(const MaterialCache&) = delete;

	/// <summary>
	/// There is only one cache.
	/// </summary>
	MaterialCache& operator=(const MaterialCache&) = delete;

	/// <summary>
	/// Clean up data.
	/// </summary>
	~MaterialCache() = default;

	/// <summary>
	/// Add a material to the cache.
	/// </summary>
	/// <param name="material">The material to add.</param>
	/// <returns>The ID of the material.</returns>
	material_id add_material(Material material) noexcept;

	/// <summary>
	/// Fetch a material by ID. If an invalid ID is provided, the default
	/// material is returned.
	/// </summary>
	/// <param name="id">The ID of the material.</param>
	/// <returns></returns>
	Material get_material(material_id id) noexcept;
private:
	/// <summary>
	/// A cache of materials.
	/// </summary>
	std::vector<Material> materials;
};