#pragma once

#include "graphics/frontend/texture.h"
#include "resource_cache/resource.h"
#include "resource_cache/resource_loader.h"

class TextureExtraData : public ResourceExtraData
{
public:
	virtual std::string to_string()
	{
		return "TextureExtraData";
	}
	std::unique_ptr<Texture> texture;
};

class TextureResourceLoader : public ResourceLoader
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
	bool parse_texture(std::shared_ptr<TextureExtraData> extra_data,
		char* raw_buffer, size_t raw_size);
};