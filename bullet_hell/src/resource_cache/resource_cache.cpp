#include "resource_cache/resource_cache.h"

#include <algorithm>

#include "debugging/logger.h"
#include "resource_cache/default_resource_loader.h"
#include "utilities/string_util.h"

bool ResourceCache::make_room(size_t size)
{
	if (size > cache_size)
	{
		return false;
	}
	while (size > (cache_size - allocated))
	{
		if (lru_list.empty())
		{
			//NOTE(ches) We ran out of things that we could free
			return false;
		}
		free_one_resource();
	}
	return true;
}

char* ResourceCache::allocate(size_t size)
{
	if (!make_room(size))
	{
		return nullptr;
	}

	char* memory = ALLOC char[size];
	if (memory)
	{
		allocated += size;
	}
	return memory;
}

void ResourceCache::free(std::shared_ptr<ResourceHandle> resource)
{
	lru_list.remove(resource);
	resources.erase(resource->resource.name);
}

std::shared_ptr<ResourceHandle> ResourceCache::load(Resource* resource)
{
	std::shared_ptr<ResourceLoader> loader;
	std::shared_ptr<ResourceHandle> handle;

	for (ResourceLoaders::iterator it = resource_loaders.begin();
		it != resource_loaders.end(); ++it)
	{
		std::shared_ptr<ResourceLoader> temp = *it;

		if (wildcard_match(temp->get_pattern().c_str(), 
			resource->name.c_str()))
		{
			loader = temp;
			break;
		}
	}

	if (!loader)
	{
		LOG_ERROR("Default resource loader was not found!");
		return handle;
	}
	
	size_t raw_size = file->get_raw_resource_size(*resource);

	if (raw_size == 0)
	{
		LOG_WARNING("Resource " + resource->name + " not found");

		return std::shared_ptr<ResourceHandle>();
	}

	size_t allocation_size = raw_size;
	if (loader->append_null())
	{
		++allocation_size;
	}

	char* raw_buffer = loader->use_raw_file() ? allocate(allocation_size) :
		ALLOC char[allocation_size];
	memset(raw_buffer, 0, allocation_size);

	if (raw_buffer == nullptr 
		|| file->load_resource(*resource, raw_buffer) == 0)
	{
		return std::shared_ptr<ResourceHandle>();
	}

	char* buffer = nullptr;
	size_t size = 0;
	if (loader->use_raw_file())
	{
		buffer = raw_buffer;
		handle = std::shared_ptr<ResourceHandle>(
			ALLOC ResourceHandle(*resource, buffer, raw_size, this));
	}
	else
	{
		size = loader->get_loaded_resource_size(raw_buffer, raw_size);
		buffer = allocate(size);
		if (raw_buffer == nullptr || buffer == nullptr)
		{
			return std::shared_ptr<ResourceHandle>();
		}
		handle = std::shared_ptr<ResourceHandle>(
			ALLOC ResourceHandle(*resource, buffer, size, this));

		bool success = loader->load_resource(raw_buffer, raw_size, handle);

		if (loader->discard_raw_buffer_after_load())
		{
			safe_delete_array(raw_buffer);
		}

		if (!success)
		{
			return std::shared_ptr<ResourceHandle>();
		}
	}

	if (handle)
	{
		lru_list.push_front(handle);
		resources[resource->name] = handle;
	}

	LOG_ASSERT(loader && "Default resource loader was not found!");
	return handle;
}

std::shared_ptr<ResourceHandle> ResourceCache::find(Resource* resource)
{
	auto result = resources.find(resource->name);
	if (result == resources.end())
	{
		return std::shared_ptr<ResourceHandle>();
	}
	return result->second;
}

void ResourceCache::update(std::shared_ptr<ResourceHandle> handle)
{
	lru_list.remove(handle);
	lru_list.push_front(handle);
}

void ResourceCache::free_one_resource()
{
	auto target = lru_list.end();
	--target;

	std::shared_ptr<ResourceHandle> handle = *target;

	lru_list.pop_back();
	resources.erase(handle->resource.name);
}

void ResourceCache::memory_has_been_freed(size_t size)
{
	allocated -= size;
}

ResourceCache::ResourceCache(const size_t size_in_MB, ResourceFile* file)
	: cache_size{ size_in_MB * 1024 * 1024 }
	, allocated{ 0 }
	, file{ file }
{}

ResourceCache::~ResourceCache()
{
	while (!lru_list.empty())
	{
		free_one_resource();
	}
	safe_delete(file);
}

bool ResourceCache::init()
{
	if (file->open())
	{
		register_loader(std::shared_ptr<ResourceLoader>(
			ALLOC DefaultResourceLoader()));
		return true;
	}
	return false;
}

void ResourceCache::register_loader(std::shared_ptr<ResourceLoader> loader)
{
	resource_loaders.push_front(loader);
}

std::shared_ptr<ResourceHandle> ResourceCache::get_handle(Resource* resource)
{
	std::shared_ptr<ResourceHandle> handle = find(resource);
	if (!handle)
	{
		handle = load(resource);
		LOG_ASSERT(handle);
	}
	else
	{
		update(handle);
	}
	return handle;
}

int ResourceCache::preload(const std::string pattern,
	ProgressCallback callback)
{
	if (file == nullptr)
	{
		return 0;
	}
	const size_t file_count = file->get_resource_count();
	int loaded = 0;
	bool cancel = false;
	for (size_t i = 0; i < file_count; ++i)
	{
		Resource resource(file->get_resource_name(i));

		if (wildcard_match(pattern.c_str(), resource.name.c_str()))
		{
			std::shared_ptr<ResourceHandle> handle = get_handle(&resource);
			++loaded;
		}

		if (callback != nullptr)
		{
			callback(static_cast<int>(i * 100 / file_count), cancel);
		}
	}
	return loaded;
}

std::vector<std::string> ResourceCache::match(const std::string pattern)
{
	std::vector<std::string> matching_names;
	if (file == nullptr)
	{
		return matching_names;
	}

	size_t file_count = file->get_resource_count();
	for (size_t i = 0; i < file_count; ++i)
	{
		std::string name = file->get_resource_name(i);
		std::transform(name.begin(), name.end(), name.begin(), std::tolower);
		if (wildcard_match(pattern.c_str(), name.c_str()))
		{
			matching_names.push_back(name);
		}
	}
	return matching_names;
}

void ResourceCache::flush()
{
	while (!lru_list.empty())
	{
		std::shared_ptr<ResourceHandle> handle = *(lru_list.begin());
		free(handle);
		lru_list.pop_front();
	}
}
