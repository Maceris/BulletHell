#pragma once

#include "graphics/graph/animation.h"
#include "resource_cache/resource.h"
#include "resource_cache/resource_loader.h"

class AnimationExtraData : public ResourceExtraData
{
public:
	virtual std::string to_string()
	{
		return "AnimationExtraData";
	}
	std::shared_ptr<Animation> animation;
};

class AnimationLoader : public ResourceLoader
{
public:
	virtual bool discard_raw_buffer_after_load()
	{
		return true;
	}

	virtual size_t get_loaded_resource_size(char* raw_buffer,
		size_t raw_size);
	virtual std::string get_pattern();
	virtual bool load_resource(char* raw_buffer, size_t raw_size,
		std::shared_ptr<ResourceHandle> handle);

	virtual bool use_raw_file()
	{
		return false;
	}
private:
	bool parse_animation(std::shared_ptr<AnimationExtraData> extra_data,
		char* raw_buffer, size_t raw_size);
};

/// <summary>
/// Load an animation from the resource cache.
/// </summary>
/// <param name="name">The name of the animation to load.</param>
/// <returns>A pointer to the loaded animation.</returns>
std::shared_ptr<Animation> load_animation(const std::string& name);