#include "DefaultResourceLoader.h"

bool DefaultResourceLoader::use_raw_file()
{
	return true;
}

bool DefaultResourceLoader::discard_raw_buffer_after_load()
{
	return true;
}

unsigned int DefaultResourceLoader::get_loaded_resource_size(char* raw_buffer,
	unsigned int raw_size)
{
	return raw_size;
}

bool DefaultResourceLoader::load_resource(char* raw_buffer, 
	unsigned int raw_size, std::shared_ptr<ResourceHandle> handle)
{
	return true;
}

std::string DefaultResourceLoader::get_pattern()
{
	return "*";
}
