#pragma once

#include "Globals.h"

#include "glad.h"
#include "glm.hpp"

#include <map>
#include <string>

/// <summary>
/// Allows for creation and usage of uniforms for a shader program.
/// </summary>
class UniformsMap
{
private:
	/// <summary>
	/// The shader program ID we are handling uniforms for.
	/// </summary>
	const GLuint program_id;

	/// <summary>
	/// The mapping of uniform name to integer location within the shader
	/// program.
	/// </summary>
	std::map<std::string, GLint> uniforms;

	/// <summary>
	/// Get the location of a uniform. If it cannot be found, defaults to 0, 
	/// but that is a major issue.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <returns>The uniform location.</returns>
	GLint get_uniform_location(const std::string& name);
public:

	/// <summary>
	/// Create a new uniform map for the given program ID.
	/// </summary>
	/// <param name="program_id">The shader program identifier.</param>
	UniformsMap(GLuint program_id);

	UniformsMap(const UniformsMap&) = delete;
	UniformsMap& operator=(const UniformsMap&) = delete;
	~UniformsMap() = default;

	/// <summary>
	/// Create a new uniform, which should match a name defined in the shader
	/// code.
	/// </summary>
	/// <param name="name">The name of the new uniform.</param>
	void create_uniform(const std::string& name);

	/// <summary>
	/// Set a float uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string& name, float value);

	/// <summary>
	/// Set an int uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string& name, int value);

	/// <summary>
	/// Set an unsigned int uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string& name, unsigned int value);

	/// <summary>
	/// Set a 4x4 matrix uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string& name, const glm::mat4& value);

	/// <summary>
	/// Set an array of 4x4 matrices uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	/// <param name="count">The number of matrices.</param>
	void set_uniform(const std::string& name, const glm::mat4* value,
		const unsigned int count);

	/// <summary>
	/// Set a 2d vector uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string& name, const glm::vec2& value);

	/// <summary>
	/// Set a 3d vector uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string& name, const glm::vec3& value);

	/// <summary>
	/// Set a 4d vector uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string& name, const glm::vec4& value);
};