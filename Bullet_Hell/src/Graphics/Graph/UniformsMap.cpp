#include "Graphics/Graph/UniformsMap.h"
#include "gtc/type_ptr.hpp"

#include "Debugging/Logger.h"

#include "glad.h"

UniformsMap::UniformsMap(GLuint program_id)
	: program_id{ program_id }
{}

GLint UniformsMap::get_uniform_location(const std::string& name)
{
	auto result = uniforms.find(name);
	LOG_ASSERT(result != uniforms.end() 
		&& std::string("Uniform " + name + " is missing").c_str());
	return result->second;
}

void UniformsMap::create_uniform(const std::string& name)
{
	GLint location = glGetUniformLocation(program_id, name.c_str());
	LOG_ASSERT(location >= 0 
		&& std::string("Uniform " + name + " is missing").c_str());
	uniforms.emplace(name, location);
}

void UniformsMap::set_uniform(const std::string& name, float value)
{
	glUniform1f(get_uniform_location(name), value);
}

void UniformsMap::set_uniform(const std::string& name, int value)
{
	glUniform1i(get_uniform_location(name), value);
}

void UniformsMap::set_uniform(const std::string& name, unsigned int value)
{
	glUniform1ui(get_uniform_location(name), value);
}

void UniformsMap::set_uniform(const std::string& name, const glm::mat4& value)
{
	glUniformMatrix4fv(get_uniform_location(name), 1, false,
		glm::value_ptr(value));
}

void UniformsMap::set_uniform(const std::string& name, const glm::mat4* value,
	const unsigned int count)
{
	glUniformMatrix4fv(get_uniform_location(name), count, false,
		glm::value_ptr(value[0]));
}

void UniformsMap::set_uniform(const std::string& name, const glm::vec2& value)
{
	glUniform2f(get_uniform_location(name), value.x, value.y);
}

void UniformsMap::set_uniform(const std::string& name, const glm::vec3& value)
{
	glUniform3f(get_uniform_location(name), value.x, value.y, value.z);
}

void UniformsMap::set_uniform(const std::string& name, const glm::vec4& value)
{
	glUniform4f(get_uniform_location(name), value.x, value.y, value.z,
		value.w);
}