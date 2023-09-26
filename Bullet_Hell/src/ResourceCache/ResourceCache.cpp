#include "ResourceCache.h"

#include "StringUtil.h"
#include "ZipFile.h"

//=============================================================================
// Resource
//=============================================================================

Resource::Resource(const std::string& resource_name)
{
	name = resource_name;
	std::transform(name.begin(), name.end(), name.begin(), std::tolower);
}

//=============================================================================
// ResourceZipFile
//=============================================================================

ResourceZipFile::ResourceZipFile(const std::string resource_file_name)
	: zip_file(nullptr)
	, resource_file_name(resource_file_name)
{}

ResourceZipFile::~ResourceZipFile()
{
	zip_file = nullptr;
}

bool ResourceZipFile::open()
{
	zip_file = ZipFile::Open(resource_file_name);
	if (!zip_file)
	{
		return false;
	}
	return true;
}

int ResourceZipFile::get_raw_resource_size(const Resource& resource)
{
	int size = 0;
	if (zip_file == nullptr)
	{
		return size;
	}
	std::shared_ptr<ZipArchiveEntry> entry = zip_file->GetEntry(resource.name);
	if (!entry)
	{
		return -1;
	}
	return entry->GetSize();
}

int ResourceZipFile::load_resource(const Resource& resource, char* buffer)
{
	int size = 0;
	if (zip_file == nullptr || buffer == nullptr)
	{
		return size;
	}

	std::shared_ptr<ZipArchiveEntry> entry = zip_file->GetEntry(resource.name);
	if (!entry || !entry->CanExtract())
	{
		return size;
	}
	size = entry->GetSize();

	std::istream* data_stream = entry->GetDecompressionStream();
	data_stream->read(buffer, size);

	return size;
}

const int ResourceZipFile::get_resource_count()
{
	if (zip_file == nullptr)
	{
		return 0;
	}
	return zip_file->GetEntriesCount();
}

std::string ResourceZipFile::get_resource_name(int index)
{
	std::string result = "";

	if (zip_file != nullptr 
		&& index >= 0 && index < zip_file->GetEntriesCount())
	{
		result = zip_file->GetEntry(index)->GetFullName();
	}
	return result;
}

//=============================================================================
// ResourceHandle
//=============================================================================

const std::string ResourceHandle::get_name()
{
	return resource.name;
}

unsigned int ResourceHandle::get_size()
{
	return size;
}

const char* ResourceHandle::get_buffer()
{
	return buffer;
}

char* ResourceHandle::get_writeable_buffer()
{
	return buffer;
}

std::shared_ptr<ResourceExtraData> ResourceHandle::get_extra()
{
	return extra;
}

void ResourceHandle::set_extra(std::shared_ptr<ResourceExtraData> extra)
{
	this->extra = extra;
}

ResourceHandle::ResourceHandle(Resource& resource, char* buffer,
	unsigned int size, ResourceCache* resource_cache)
	: resource(resource)
	, buffer(buffer)
	, size(size)
	, extra(std::shared_ptr<ResourceExtraData>())
	, resource_cache(resource_cache)
{}

ResourceHandle::~ResourceHandle()
{
	SAFE_DELETE_ARRAY(buffer);
	resource_cache->memory_has_been_freed(size);
}


//=============================================================================
// ResourceLoader
//=============================================================================

bool ResourceLoader::append_null()
{
	return false;
}

//=============================================================================
// DefaultResourceLoader
//=============================================================================

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

bool DefaultResourceLoader::load_resource(char* raw_buffer, unsigned int raw_size,
	std::shared_ptr<ResourceHandle> handle)
{
	return true;
}

std::string DefaultResourceLoader::get_pattern()
{
	return "*";
}

//=============================================================================
// ResourceCache
//=============================================================================


bool ResourceCache::make_room(unsigned int size)
{
	if (size > cache_size)
	{
		return false;
	}
	while (size > (cache_size - allocated))
	{
		if (lru_list.empty())
		{
			// We ran out of things that we could free
			return false;
		}
		free_one_resource();
	}
	return true;
}

char* ResourceCache::allocate(unsigned int size)
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
		//TODO(ches) log this, we should have at least the default loader
		return handle;
	}

	int raw_size = file->get_raw_resource_size(*resource);

	if (raw_size < 0)
	{
		//TODO(ches) log this
		return std::shared_ptr<ResourceHandle>();
	}

	int allocation_size = raw_size;
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
	unsigned int size = 0;
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
			SAFE_DELETE_ARRAY(raw_buffer);
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

	//TODO(ches) log this, we are out of memory or don't have a default loader
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

void ResourceCache::memory_has_been_freed(unsigned int size)
{
	allocated -= size;
}

ResourceCache::ResourceCache(const unsigned int size_in_MB, ResourceFile* file)
	: cache_size(size_in_MB * 1024 * 1024)
	, allocated(0)
	, file(file)
{}

ResourceCache::~ResourceCache()
{
	while (!lru_list.empty())
	{
		free_one_resource();
	}
	SAFE_DELETE(file);
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
		//TODO(ches) double check we actually loaded anything
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
	int file_count = file->get_resource_count();
	int loaded = 0;
	bool cancel = false;
	for (int i = 0; i < file_count; ++i)
	{
		Resource resource(file->get_resource_name(i));

		if (wildcard_match(pattern.c_str(), resource.name.c_str()))
		{
			std::shared_ptr<ResourceHandle> handle = get_handle(&resource);
			++loaded;
		}

		if (callback != nullptr)
		{
			callback(i * 100 / file_count, cancel);
		}
	}
}

std::vector<std::string> ResourceCache::match(const std::string pattern)
{
	std::vector<std::string> matching_names;
	if (file == nullptr)
	{
		return matching_names;
	}

	int file_count = file->get_resource_count();
	for (int i = 0; i < file_count; ++i)
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
