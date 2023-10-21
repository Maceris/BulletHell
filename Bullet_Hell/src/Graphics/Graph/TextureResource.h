#pragma once

#include "Globals.h"

#include "Resource.h"
#include "ResourceLoader.h"
#include "Texture.h"

class TextureExtraData : public ResourceExtraData
{
public:
	virtual std::string to_string()
	{
		return "TextureExtraData";
	}
	std::shared_ptr<Texture> texture;
};

class TextureLoader : public ResourceLoader
{
public:
	virtual bool discard_raw_buffer_after_load()
	{
		return true;
	}

	virtual unsigned int get_loaded_resource_size(char* raw_buffer,
		unsigned int raw_size);
	virtual std::string get_pattern();
	virtual bool load_resource(char* raw_buffer, unsigned int raw_size,
		std::shared_ptr<ResourceHandle> handle);

	virtual bool use_raw_file()
	{
		return false;
	}
private:
	bool parse_texture(std::shared_ptr<TextureExtraData> extra_data,
		char* raw_buffer, unsigned int raw_size);
};

/// <summary>
/// Load a texture from the resource cache.
/// </summary>
/// <param name="name">The name of the texture to load.</param>
/// <returns>A pointer to the loaded texture.</returns>
std::shared_ptr<Texture> load_texture(const std::string& name);
