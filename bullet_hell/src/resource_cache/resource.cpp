#include "resource_cache/resource.h"

#include <algorithm>

Resource::Resource(const std::string& resource_name)
	: name{ resource_name }
{
	std::transform(name.begin(), name.end(), name.begin(), std::tolower);
}
