#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include <format>
#include <map>

#include "glm/gtc/type_ptr.hpp"

#include "graphics/backend/base/uniforms_map.h"
#include "debugging/logger.h"

#include "glad.h"

struct UniformsMap::Data
{
	Data(ShaderHandle program_id)
		: program_id{ static_cast<GLuint>(program_id) }
		, uniforms{}
	{}
	Data(const Data&) = delete;
	Data& operator=(const Data&) = delete;
	~Data() = default;

	/// <summary>
	/// The shader program ID we are handling uniforms for.
	/// </summary>
	const GLuint program_id;

	/// <summary>
	/// The mapping of uniform name to integer location within the shader
	/// program.
	/// </summary>
	std::map<std::string, GLint> uniforms;
};

UniformsMap::UniformsMap(ShaderHandle program_id)
	: data{ std::make_unique<Data>(program_id) }
{}

UniformsMap::~UniformsMap() = default;

void UniformsMap::create_uniform(const std::string_view name)
{
	GLint location = glGetUniformLocation(data->program_id, name.data());
	LOG_ASSERT(location >= 0 && std::format("Uniform {} is missing", name).c_str());
	data->uniforms.emplace(name, location);
}

void UniformsMap::set_uniform(const std::string_view name, float value)
{

}

void UniformsMap::set_uniform(const std::string_view name, int value)
{

}

void UniformsMap::set_uniform(const std::string_view name, const glm::mat4& value)
{

}

void UniformsMap::set_uniform(const std::string_view name, const glm::vec2& value)
{

}

void UniformsMap::set_uniform(const std::string_view name, const glm::vec3& value)
{

}

void UniformsMap::set_uniform(const std::string_view name, const glm::vec4& value)
{

}


#endif