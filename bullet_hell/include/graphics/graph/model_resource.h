#pragma once

#include "graphics/graph/model.h"
#include "resource_cache/resource.h"
#include "resource_cache/resource_loader.h"

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
	bool parse_model(std::shared_ptr<ModelExtraData> extra_data,
		char* raw_buffer, size_t raw_size, const std::string& file_name);
};

/// <summary>
/// Load a model from the resource cache.
/// </summary>
/// <param name="name">The name of the model to load.</param>
/// <returns>A pointer to the loaded model.</returns>
std::shared_ptr<Model> load_model(const std::string& name);