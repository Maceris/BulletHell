#pragma once

#include "Globals.h"

#include "Material.h"
#include "Resource.h"
#include "ResourceLoader.h"

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
	bool parse_material(std::shared_ptr<MaterialExtraData> extra_data,
		char* raw_buffer, unsigned int raw_size);
};