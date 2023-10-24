#pragma once

/*
	NOTE(ches) This file is not intended to be included directly. Please see
	ResourceCache.h instead.
*/

#include "ResourceLoader.h"
#include "ResourceHandle.h"

/// <summary>
/// The default resource loader, which just loads a resource exactly as it is
/// in the file.
/// </summary>
class DefaultResourceLoader : public ResourceLoader
{
	virtual bool discard_raw_buffer_after_load();
	virtual size_t get_loaded_resource_size(char* raw_buffer,
		size_t raw_size);
	virtual std::string get_pattern();
	virtual bool load_resource(char* raw_buffer, size_t raw_size,
		std::shared_ptr<ResourceHandle> handle);
	virtual bool use_raw_file();
};
