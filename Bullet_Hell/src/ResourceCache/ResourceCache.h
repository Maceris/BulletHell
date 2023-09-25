#pragma once

#include "Globals.h"

#include <list>
#include <map>
#include <memory>
#include <string>

#include "ZipArchive.h"

/// <summary>
/// Extra data associated with a resource. For example, length and format of
/// a sound file.
/// </summary>
class ResourceExtraData
{
public:
	/// <summary>
	/// Return a string version of the extra data.
	/// </summary>
	/// <returns>The extra data.</returns>
	virtual std::string to_string() = 0;
};

/// <summary>
/// A unique identifier of a resource.
/// </summary>
class Resource
{
public:
	/// <summary>
	/// The name of the resource, typically including the path to the resource.
	/// </summary>
	std::string name;

	/// <summary>
	/// Create a new resource.
	/// </summary>
	/// <param name="resource_name">The name of the resource, typically
	/// including the path to the resource.</param>
	Resource(const std::string& resource_name);
};

/// <summary>
/// A file that can be opened and closed, and provides the application with
/// resources.
/// </summary>
class ResourceFile {
public:
	/// <summary>
	/// Open the file and return success or failure.
	/// </summary>
	/// <returns>Whether we opened the file without error.</returns>
	virtual bool open() = 0;

	/// <summary>
	/// Fetch the size of a resource based on the name. If the resource is not
	/// found, returns -1.
	/// </summary>
	/// <param name="resource">The resource we want the size of.</param>
	/// <returns>The size, in bytes.</returns>
	virtual int get_raw_resource_size(const Resource& resource) = 0;

	/// <summary>
	/// Read the resource from the file.
	/// </summary>
	/// <param name="resource">The resource to read.</param>
	/// <param name="buffer">The buffer to load into.</param>
	/// <returns>The size of the loaded resource, in bytes.</returns>
	virtual int load_resource(const Resource& resource, char* buffer) = 0;

	/// <summary>
	/// Calculates the number of resources that are in the file.
	/// </summary>
	/// <returns>The number of resources in the file.</returns>
	virtual const int get_resource_count() = 0;

	/// <summary>
	/// Fetches the name of the n'th resource in the file.
	/// </summary>
	/// <param name="index">The index of the resource.</param>
	/// <returns>The name of the resource at the supplied index.
	/// Empty if index is not in bounds.
	/// </returns>
	virtual std::string get_resource_name(int index) = 0;

	/// <summary>
	/// Clean up.
	/// </summary>
	virtual ~ResourceFile() { }
};

/// <summary>
/// A resource file that loads from a .zip file.
/// </summary>
class ResourceZipFile : public ResourceFile
{
private:
	/// <summary>
	/// A reference to the zip file we are loading from.
	/// </summary>
	std::shared_ptr<ZipArchive> zip_file;

	/// <summary>
	/// The name of the zip file, including the path.
	/// </summary>
	std::string resource_file_name;
public:
	/// <summary>
	/// Create a new resource zip file.
	/// </summary>
	/// <param name="resource_file_name">The name of the zip file, including
	/// path.</param>
	ResourceZipFile(const std::string resource_file_name);

	/// <summary>
	/// Clean up.
	/// </summary>
	virtual ~ResourceZipFile();

	/// <summary>
	/// Open the file and return success or failure.
	/// </summary>
	/// <returns>Whether we opened the file without error.</returns>
	virtual bool open();

	/// <summary>
	/// Fetch the size of a resource based on the name. If the resource is not
	/// found, returns -1.
	/// </summary>
	/// <param name="resource">The resource we want the size of.</param>
	/// <returns>The size, in bytes.</returns>
	virtual int get_raw_resource_size(const Resource& resource);

	/// <summary>
	/// Read the resource from the file. If the resource is not
	/// found, returns 0.
	/// </summary>
	/// <param name="resource">The resource to read.</param>
	/// <param name="buffer">The pre-allocated buffer to load into.</param>
	/// <returns>The size of the loaded resource, in bytes.</returns>
	virtual int load_resource(const Resource& resource, char* buffer);

	/// <summary>
	/// Calculates the number of resources that are in the file.
	/// </summary>
	/// <returns>The number of resources in the file.</returns>
	virtual const int get_resource_count();

	/// <summary>
	/// Fetches the name of the n'th resource in the file.
	/// </summary>
	/// <param name="index">The index of the resource.</param>
	/// <returns>The name of the resource at the supplied index.
	/// Empty if index is not in bounds.
	/// </returns>
	virtual std::string get_resource_name(int index);
};

/// <summary>
/// A resource that is loaded from file. Also tracks detials like its size and
/// which resource cache was used to load it.
/// </summary>
class ResourceHandle
{
	friend class ResourceCache;

protected:
	/// <summary>
	/// The name of the resource.
	/// </summary>
	Resource resource;

	/// <summary>
	/// The actual data.
	/// </summary>
	char* buffer;

	/// <summary>
	/// The size of the data, in bytes.
	/// </summary>
	unsigned int size;

	/// <summary>
	/// The optional extra data.
	/// </summary>
	std::shared_ptr<ResourceExtraData> extra;

	/// <summary>
	/// The resource cache that was used to load this resource.
	/// </summary>
	ResourceCache* resource_cache;

	/// <summary>
	/// Create a new resource handle.
	/// </summary>
	/// <param name="resource">The name of the resource.</param>
	/// <param name="buffer">The data.</param>
	/// <param name="size">The size of the data, in bytes.</param>
	/// <param name="resource_cache">The resource cache used to load 
	/// this resource.</param>
	ResourceHandle(Resource& resource, char* buffer, unsigned int size,
		ResourceCache* resource_cache);

public:

	/// <summary>
	/// Clean up and notify the resource cache that memory has been freed.
	/// </summary>
	virtual ~ResourceHandle();

	/// <summary>
	/// Return the name of this resource.
	/// </summary>
	/// <returns>The unique name of this resource.</returns>
	const std::string get_name();

	/// <summary>
	/// Return the size of this resource, in bytes.
	/// </summary>
	/// <returns>The size of the data.</returns>
	unsigned int get_size();

	/// <summary>
	/// Return a const reference to the data.
	/// </summary>
	/// <returns>The raw data for this resource.</returns>
	const char* get_buffer();

	/// <summary>
	/// Return a writeable reference to the data.
	/// </summary>
	/// <returns>The raw data for this resource.</returns>
	char* get_writeable_buffer();

	/// <summary>
	/// Return the extra data for this resource. Will point to nullptr if 
	/// there is no extra data.
	/// </summary>
	/// <returns>A pointer to extra data, if there is any.</returns>
	std::shared_ptr<ResourceExtraData> get_extra();

	/// <summary>
	/// Set the extra data. The pointer must not be itself null, but may point
	/// to null.
	/// </summary>
	/// <param name="extra">The extra data.</param>
	void set_extra(std::shared_ptr<ResourceExtraData> extra);
};

/// <summary>
/// Used to load resources from file. There may be several resource loaders,
/// which do special processing for certian types of files.
/// </summary>
class ResourceLoader
{
public:
	/// <summary>
	/// Return the wildcard pattern that defines which resources the loader
	/// is used for. For example, "*.ogg" is any ogg file, and "*" is any file
	/// at all.
	/// </summary>
	/// <returns>The pattern that defines which files this is used for.
	/// </returns>
	virtual std::string get_pattern() = 0;

	/// <summary>
	/// Whether we can use the bits stored in the raw file, without any
	/// processing.
	/// </summary>
	/// <returns>Whether we can use the raw data without processing.</returns>
	virtual bool use_raw_file() = 0;

	/// <summary>
	/// Whether we discard the raw buffer after loading. This is the case for
	/// things like compressed data that we have to unpack, or transform, 
	/// as we don't want to keep two copies of the data around.
	/// </summary>
	/// <returns>Whether the raw data is dicarded after loading.</returns>
	virtual bool discard_raw_buffer_after_load() = 0;

	/// <summary>
	/// Whether we append a null/zero at the end of the data.
	/// </summary>
	/// <returns>If a null gets appended to the end of the data.</returns>
	virtual bool append_null();

	/// <summary>
	/// Returns the size of the loaded resource, which might be different from
	/// the size stored in the file.
	/// </summary>
	/// <param name="raw_buffer">The raw data.</param>
	/// <param name="raw_size">The size of the raw data.</param>
	/// <returns>The size of the loaded resource.</returns>
	virtual unsigned int get_loaded_resource_size(char* raw_buffer,
		unsigned int raw_size) = 0;

	/// <summary>
	/// Load a resource.
	/// </summary>
	/// <param name="raw_buffer">The raw buffer to load into.</param>
	/// <param name="raw_size">The size of the buffer.</param>
	/// <param name="handle">The handle that we want to load.</param>
	/// <returns>Whether we loaded the resource successfully.</returns>
	virtual bool load_resource(char* raw_buffer, unsigned int raw_size,
		std::shared_ptr<ResourceHandle> handle) = 0;
};

/// <summary>
/// The default resource loader, which just loads a resource exactly as it is
/// in the file.
/// </summary>
class DefaultResourceLoader : public ResourceLoader
{
	/// <summary>
	/// Whether we can use the bits stored in the raw file, without any
	/// processing.
	/// </summary>
	/// <returns>Whether we can use the raw data without processing.</returns>
	virtual bool use_raw_file();

	/// <summary>
	/// Whether we discard the raw buffer after loading. This is the case for
	/// things like compressed data that we have to unpack, or transform, 
	/// as we don't want to keep two copies of the data around.
	/// </summary>
	/// <returns>Whether the raw data is dicarded after loading.</returns>
	virtual bool discard_raw_buffer_after_load();

	/// <summary>
	/// Returns the size of the loaded resource, which might be different from
	/// the size stored in the file.
	/// </summary>
	/// <param name="raw_buffer">The raw data.</param>
	/// <param name="raw_size">The size of the raw data.</param>
	/// <returns>The size of the loaded resource.</returns>
	virtual unsigned int get_loaded_resource_size(char* raw_buffer,
		unsigned int raw_size);

	/// <summary>
	/// Load a resource.
	/// </summary>
	/// <param name="raw_buffer">The raw buffer to load into.</param>
	/// <param name="raw_size">The size of the buffer.</param>
	/// <param name="handle">The handle that we want to load.</param>
	/// <returns>Whether we loaded the resource successfully.</returns>
	virtual bool load_resource(char* raw_buffer, unsigned int raw_size,
		std::shared_ptr<ResourceHandle> handle);

	/// <summary>
	/// Return the wildcard pattern that defines which resources the loader
	/// is used for. For example, "*.ogg" is any ogg file, and "*" is any file
	/// at all.
	/// </summary>
	/// <returns>The pattern that defines which files this is used for.
	/// </returns>
	virtual std::string get_pattern();
};

using ResourceHandleList = std::list<std::shared_ptr<ResourceHandle>>;
using ResourceHandleMap = std::map<std::string, std::shared_ptr<ResourceHandle>>;
using ResourceLoaders = std::list<std::shared_ptr<ResourceLoader>>;

using ProgressCallback = void (*)(int, bool&);

class ResourceCache
{
	friend class ResourceHandle;

	/// <summary>
	/// The list of resource handles that are currently loaded. These are 
	/// stored in least-recently-used order, so every time a resource is used
	/// it gets moved to the front of the list.
	/// </summary>
	ResourceHandleList lru_list;

	/// <summary>
	/// Used to look up resource handles by name.
	/// </summary>
	ResourceHandleMap resources;

	/// <summary>
	/// A list of resource loaders. The most specific loaders should come
	/// first on the list, with more generic loaders last on the list.
	/// 
	/// Registering loaders pushes them to the front of the list.
	/// </summary>
	ResourceLoaders resource_loaders;

	/// <summary>
	/// The resource file that we load from.
	/// </summary>
	ResourceFile* file;

	/// <summary>
	/// The amount of cache memory in use.
	/// </summary>
	unsigned int cache_size;

	/// <summary>
	/// The total amount of memory allocated for the cache.
	/// </summary>
	unsigned int allocated;

protected:
	
	/// <summary>
	/// Attempt to make room in the cache by freeing memory. If there is no
	/// possible way to free memory, we return false.
	/// </summary>
	/// <param name="size">The number of bytes we want to make room for.
	/// </param>
	/// <returns>Whether we have the specified amount of room.</returns>
	bool make_room(unsigned int size);

	/// <summary>
	/// Allocate raw memory of the specified size. If we did not have room
	/// to do so, null will be returned.
	/// </summary>
	/// <param name="size">The number of bytes to allocate.</param>
	/// <returns>The allocated memory, or null in the worst case.</returns>
	char* allocate(unsigned int size);

	/// <summary>
	/// Looks up a resource by handle and remove it from the cache.
	/// 
	/// The cache will only count the memory as freed once the resource
	/// pointed to by the handle is destroyed.
	/// </summary>
	/// <param name="resource">The resource to free.</param>
	void free(std::shared_ptr<ResourceHandle> resource);

	/// <summary>
	/// Loads a resource and returns the handle to it. If the resource cannot
	/// be found, or can't be laoded, an empty pointer is returned.
	/// </summary>
	/// <param name="resource">The resource we want to load.</param>
	/// <returns>The handle for the loaded resource, empty on failure.
	/// </returns>
	std::shared_ptr<ResourceHandle> load(Resource* resource);

	/// <summary>
	/// Looks up the resource handle for a resource. If no resource handle is
	/// found, an empty pointer is returned.
	/// </summary>
	/// <param name="resource">The resource to search for.</param>
	/// <returns>A pointer to the appropriate handle.</returns>
	std::shared_ptr<ResourceHandle> find(Resource* resource);

	/// <summary>
	/// Moves a handle to the front of the LRU list, to maintain the desired
	/// sorting.
	/// </summary>
	/// <param name="handle">The handle to update in the LRU tracking.</param>
	void update(std::shared_ptr<ResourceHandle> handle);

	/// <summary>
	/// Free the least recently used resource.
	/// 
	/// The cache will only count the memory as freed once the resource
	/// pointed to by the handle is destroyed.
	/// </summary>
	void free_one_resource();

	/// <summary>
	/// Called whenever memory associated with a resource has actually been
	/// freed, so that the cache can track allocation appropriately.
	/// </summary>
	/// <param name="size"></param>
	void memory_has_been_freed(unsigned int size);

public:
	/// <summary>
	/// Create a resource cache for a file.
	/// </summary>
	/// <param name="size_in_MB">The amount of memory allocated to the 
	/// cache, in megabytes.</param>
	/// <param name="file">The file we are loading resources from.</param>
	ResourceCache(const unsigned int size_in_MB, ResourceFile* file);

	/// <summary>
	/// Clean up.
	/// </summary>
	virtual ~ResourceCache();

	/// <summary>
	/// Initializes the resource cache, preparing to load resources. If we
	/// had an issue, like the file already being in use by another cache
	/// instance, we will return false.
	/// </summary>
	/// <returns>Whether we initialized successfully.</returns>
	bool init();

	/// <summary>
	/// Register a resource loader, placing it at the front of the list of 
	/// loaders.
	/// 
	/// This way a loader for a specific file could be used, while
	/// still having more generic loaders for other files of the same
	/// extension.
	/// </summary>
	/// <param name="loader">The loader to register.</param>
	void register_loader(std::shared_ptr<ResourceLoader> loader);

	/// <summary>
	/// If a resource is already in the cache, return it. Otherwise, we load
	/// the resource from file first.
	/// 
	/// If we had an error, like running out of memory, an empty pointer is
	/// returned.
	/// </summary>
	/// <param name="resource">The resource to fetch.</param>
	/// <returns>A handle to the resource.</returns>
	std::shared_ptr<ResourceHandle> get_handle(Resource* resource);

	/// <summary>
	/// Pre-load a set of resources matching the specified wildcard pattern.
	/// For example, a pattern of "*.jpg" loads all the jpeg files in the 
	/// resource file.
	/// 
	/// This takes in a callback which will be notified with the percentage
	/// progress, and whether we have canceled loading.
	/// </summary>
	/// <param name="pattern">The wildcard pattern specifying which 
	/// resources to load.</param>
	/// <param name="callback">A callback to be updated regarding progress.
	/// May be null if progress does not need to be tracked.</param>
	/// <returns>The number of resources that were loaded.</returns>
	int preload(const std::string pattern, ProgressCallback callback);

	/// <summary>
	/// Searches through the cache for assets that match the specified 
	/// wildcard pattern.
	/// </summary>
	/// <param name="pattern">The wildcard pattern to look for.</param>
	/// <returns>A list of resource names that match the given pattern.
	/// </returns>
	std::vector<std::string> match(const std::string pattern);

	/// <summary>
	/// Free every handle in the cache. Useful for loading a new level or 
	/// debugging.
	/// </summary>
	void flush();
};
