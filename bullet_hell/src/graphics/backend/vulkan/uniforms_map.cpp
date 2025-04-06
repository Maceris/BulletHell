#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <format>
#include <map>

#include "glm/gtc/type_ptr.hpp"

#include "graphics/frontend/uniforms_map.h"
#include "debugging/logger.h"

//TODO(ches) fill this out

struct UniformsMap::Data
{
	Data(ShaderHandle program_id)
	{}
	Data(const Data&) = delete;
	Data& operator=(const Data&) = delete;
	~Data() = default;

};

UniformsMap::UniformsMap(ShaderHandle program_id)
	: data{ std::make_unique<Data>(program_id) }
{}

UniformsMap::~UniformsMap() = default;

void UniformsMap::create_uniform(const std::string_view name)
{
	
}

void UniformsMap::set_uniform(const std::string_view name, float value)
{
}

void UniformsMap::set_uniform(const std::string_view name, int value)
{
}

void UniformsMap::set_uniform(const std::string_view name, unsigned int value)
{
}

void UniformsMap::set_uniform(const std::string_view name,
	const glm::mat4& value)
{

}

void UniformsMap::set_uniform(const std::string_view name,
	const glm::vec2& value)
{
}

void UniformsMap::set_uniform(const std::string_view name,
	const glm::vec3& value)
{
}

void UniformsMap::set_uniform(const std::string_view name,
	const glm::vec4& value)
{
}

#endif