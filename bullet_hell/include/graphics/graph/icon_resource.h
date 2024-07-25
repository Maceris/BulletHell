#pragma once

#include "globals.h"

#include "resource_cache/resource.h"
#include "resource_cache/resource_loader.h"

struct Icon
{
	int width;
	int height;
	unsigned char* data;

	Icon(const int& width, const int& height, unsigned char* data)
		: width{ width }
		, height{ height }
		, data{ data }
	{}
	Icon(const Icon&) = delete;
	Icon& operator=(const Icon&) = delete;
	~Icon()
	{
		safe_delete(data);
	}
};

class IconExtraData : public ResourceExtraData
{
public:
	virtual std::string to_string()
	{
		return "IconExtraData";
	}
	std::shared_ptr<Icon> icon;
};

class IconLoader : public ResourceLoader
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
	bool parse_icon(std::shared_ptr<IconExtraData> extra_data,
		char* raw_buffer, size_t raw_size);
};

/// <summary>
/// Load an icon texture from the resource cache.
/// </summary>
/// <param name="name">The name of the icon to load.</param>
/// <returns>A pointer to the loaded icon.</returns>
std::shared_ptr<Icon> load_icon(const std::string& name);
