#pragma once

/*
	NOTE(ches) This file is not intended to be included directly. Please see
	ResourceCache.h instead.
*/

#include <memory>

#include "ResourceFile.h"
#include "ZipArchive.h"

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
	virtual size_t get_raw_resource_size(const Resource& resource);

	/// <summary>
	/// Read the resource from the file. If the resource is not
	/// found, returns 0.
	/// </summary>
	/// <param name="resource">The resource to read.</param>
	/// <param name="buffer">The pre-allocated buffer to load into.</param>
	/// <returns>The size of the loaded resource, in bytes.</returns>
	virtual size_t load_resource(const Resource& resource, char* buffer);

	/// <summary>
	/// Calculates the number of resources that are in the file.
	/// </summary>
	/// <returns>The number of resources in the file.</returns>
	virtual const size_t get_resource_count();

	/// <summary>
	/// Fetches the name of the n'th resource in the file.
	/// </summary>
	/// <param name="index">The index of the resource.</param>
	/// <returns>The name of the resource at the supplied index.
	/// Empty if index is not in bounds.
	/// </returns>
	virtual std::string get_resource_name(size_t index);
};
