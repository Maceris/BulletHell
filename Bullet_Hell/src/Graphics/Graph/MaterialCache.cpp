#include "MaterialCache.h"

MaterialCache::MaterialCache()
	: materials(std::vector<Material>())
{
	// We always want a default texture to exist, as a fallback
	materials.emplace_back();
}

material_id MaterialCache::add_material(Material material) noexcept
{
	const int assigned_index = materials.size();
	materials.push_back(material);
	return assigned_index;
}

Material MaterialCache::get_material(material_id id) noexcept
{
	if (id < 0 || id >= materials.size()) {
		// Fallback
		return materials[0];
	}
	return materials[id];
}