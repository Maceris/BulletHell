#pragma once

#include "Globals.h"

#include "Model.h"
#include "Resource.h"
#include "ResourceLoader.h"

class ModelExtraData : public ResourceExtraData
{
public:
	virtual std::string to_string()
	{
		return "ModelExtraData";
	}
	std::shared_ptr<Model> model;
};

class ModelLoader : public ResourceLoader
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
	bool parse_model(std::shared_ptr<ModelExtraData> extra_data,
		char* raw_buffer, unsigned int raw_size, const std::string& file_name);
};