#pragma once

#include "Globals.h"

#include "Map.h"
#include "Resource.h"
#include "ResourceLoader.h"

class MapExtraData : public ResourceExtraData
{
public:
	virtual std::string to_string()
	{
		return "MapExtraData";
	}
	std::shared_ptr<Map> map;
};

class MapLoader : public ResourceLoader
{
public:
	virtual bool discard_raw_buffer_after_load()
	{
		return true;
	}

	virtual size_t get_loaded_resource_size(char* raw_buffer, size_t raw_size);
	virtual std::string get_pattern();
	virtual bool load_resource(char* raw_buffer, size_t raw_size,
		std::shared_ptr<ResourceHandle> handle);

	virtual bool use_raw_file()
	{
		return false;
	}
private:
	bool parse_map(std::shared_ptr<MapExtraData> extra_data,
		char* raw_buffer, size_t raw_size);
};

/// <summary>
/// Load a map from the resource cache.
/// </summary>
/// <param name="name">The name of the map to load.</param>
/// <returns>A pointer to the loaded map.</returns>
std::shared_ptr<Map> load_map(const std::string& name);