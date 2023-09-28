#pragma once

#include "Globals.h"

#include "glad.h"
#include "glm.hpp"

#include <map>
#include <string>

class UniformsMap
{
private:
	const int program_id;

	std::map<std::string, GLint> uniforms;

	GLint get_uniform_location(const std::string& name);
public:
	UniformsMap(GLuint program_id);
	UniformsMap(const UniformsMap&) = delete;
	UniformsMap& operator=(const UniformsMap&) = delete;
	~UniformsMap() = default;

	void create_uniform(const std::string& name);
	void set_uniform(const std::string& name, float value);
	void set_uniform(const std::string& name, int value);
	void set_uniform(const std::string& name, const glm::mat4& value);
	void set_uniform(const std::string& name, const glm::mat4* value,
		const unsigned int count);
	void set_uniform(const std::string& name, const glm::vec2& value);
	void set_uniform(const std::string& name, const glm::vec3& value);
	void set_uniform(const std::string& name, const glm::vec4& value);
};