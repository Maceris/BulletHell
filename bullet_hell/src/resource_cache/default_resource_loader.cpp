#include "resource_cache/default_resource_loader.h"

#include "resource_cache/resource_handle.h"

bool DefaultResourceLoader::use_raw_file()
{
	return true;
}

bool DefaultResourceLoader::discard_raw_buffer_after_load()
{
	return true;
}

size_t DefaultResourceLoader::get_loaded_resource_size(char* raw_buffer,
	size_t raw_size)
{
	return raw_size;
}

bool DefaultResourceLoader::load_resource(char* raw_buffer, 
	size_t raw_size, std::shared_ptr<ResourceHandle> handle)
{
	return true;
}

std::string DefaultResourceLoader::get_pattern()
{
	return "*";
}
