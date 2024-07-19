#pragma once

#include "globals.h"

#include "graphics/graph/material.h"
#include "resource_cache/resource.h"
#include "resource_cache/resource_loader.h"

class MaterialExtraData : public ResourceExtraData
{
public:
	virtual std::string to_string()
	{
		return "MaterialExtraData";
	}
	std::shared_ptr<Material> material;
};

class MaterialLoader : public ResourceLoader
{
public:
	virtual bool discard_raw_buffer_after_load()
	{
		return true;
	}

	virtual size_t get_loaded_resource_size(char* raw_buffer,
		size_t raw_size);
	virtual std::string get_pattern();
	virtual bool load_resource(char* raw_buffer, size_t raw_size,
		std::shared_ptr<ResourceHandle> handle);

	virtual bool use_raw_file()
	{
		return false;
	}
private:
	bool parse_material(std::shared_ptr<MaterialExtraData> extra_data,
		char* raw_buffer, size_t raw_size);
};

/// <summary>
/// Load a material from the resource cache.
/// </summary>
/// <param name="name">The name of the material to load.</param>
/// <returns>A pointer to the loaded material.</returns>
std::shared_ptr<Material> load_material(const std::string& name);