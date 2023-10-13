#pragma once

/*
	NOTE(ches) This file is not intended to be included directly. Please see
	ResourceCache.h instead.
*/

#include <string>

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
