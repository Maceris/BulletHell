#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL || BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include <format>
#include <map>

#include "glm/gtc/type_ptr.hpp"

#include "graphics/frontend/uniforms_map.h"
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

GLint get_uniform_location(std::map<std::string, GLint>& uniforms,
	const std::string_view name)
{
	auto result = uniforms.find(name.data());
	LOG_ASSERT(result != uniforms.end() 
		&& std::format("Uniform {} is missing", name).c_str());
	return result->second;
}

UniformsMap::UniformsMap(ShaderHandle program_id)
	: data{ std::make_unique<Data>(program_id) }
{}

UniformsMap::~UniformsMap() = default;

void UniformsMap::create_uniform(const std::string_view name)
{
	GLint location = glGetUniformLocation(data->program_id, name.data());
	LOG_ASSERT(location >= 0 
		&& std::format("Uniform {} is missing", name).c_str());
	data->uniforms.emplace(name, location);
}

void UniformsMap::set_uniform(const std::string_view name, float value)
{
	glUniform1f(get_uniform_location(data->uniforms, name), value);
}

void UniformsMap::set_uniform(const std::string_view name, int value)
{
	glUniform1i(get_uniform_location(data->uniforms, name), value);
}

void UniformsMap::set_uniform(const std::string_view name, unsigned int value)
{
	glUniform1ui(get_uniform_location(data->uniforms, name), value);
}

void UniformsMap::set_uniform(const std::string_view name,
	const glm::mat4& value)
{
	glUniformMatrix4fv(get_uniform_location(data->uniforms, name), 1, false,
		glm::value_ptr(value));
}

void UniformsMap::set_uniform(const std::string_view name,
	const glm::vec2& value)
{
	glUniform2f(get_uniform_location(data->uniforms, name), value.x, value.y);
}

void UniformsMap::set_uniform(const std::string_view name,
	const glm::vec3& value)
{
	glUniform3f(get_uniform_location(data->uniforms, name), value.x, value.y,
		value.z);
}

void UniformsMap::set_uniform(const std::string_view name,
	const glm::vec4& value)
{
	glUniform4f(get_uniform_location(data->uniforms, name), value.x, value.y,
		value.z, value.w);
}

#endif