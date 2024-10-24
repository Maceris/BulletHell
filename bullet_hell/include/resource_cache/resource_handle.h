#pragma once

/*
	NOTE(ches) This file is not intended to be included directly. Please see
	ResourceCache.h instead.
*/

#include <memory>
#include <string>

#include "resource_cache/resource.h"

class ResourceCache;

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
	/// The actual data, assuming we keep the raw buffer.
	/// </summary>
	char* buffer;

	/// <summary>
	/// The size of the data, in bytes.
	/// </summary>
	size_t size;

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
	ResourceHandle(Resource& resource, char* buffer, size_t size,
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
	size_t get_size();

	/// <summary>
	/// Return a const reference to the data. If this type of resource does
	/// not use the raw data, this will be a nullptr.
	/// </summary>
	/// <returns>The raw data for this resource.</returns>
	const char* get_buffer();

	/// <summary>
	/// Return a writeable reference to the data. If this type of resource does
	/// not use the raw data, this will be a nullptr.
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
