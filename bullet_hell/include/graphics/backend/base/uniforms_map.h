#pragma once

#include <memory>
#include <string_view>

#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

using ShaderHandle = uint64_t;

struct UniformsMap
{
	/// <summary>
	/// Create a new uniform map for the given program ID.
	/// </summary>
	/// <param name="program_id">The shader program identifier.</param>
	UniformsMap(ShaderHandle program_id);

	UniformsMap(const UniformsMap&) = delete;
	UniformsMap& operator=(const UniformsMap&) = delete;
	~UniformsMap();

	/// <summary>
	/// Create a new uniform, which should match a name defined in the shader
	/// code.
	/// </summary>
	/// <param name="name">The name of the new uniform.</param>
	void create_uniform(const std::string_view name);

	/// <summary>
	/// Set a float uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string_view name, float value);

	/// <summary>
	/// Set an int uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string_view name, int value);

	/// <summary>
	/// Set an unsigned int uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string_view name, unsigned int value);

	/// <summary>
	/// Set a 4x4 matrix uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string_view name, const glm::mat4& value);

	/// <summary>
	/// Set an array of 4x4 matrices uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	/// <param name="count">The number of matrices.</param>
	void set_uniform(const std::string_view name, const glm::mat4* value,
		const unsigned int count);

	/// <summary>
	/// Set a 2d vector uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string_view name, const glm::vec2& value);

	/// <summary>
	/// Set a 3d vector uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string_view name, const glm::vec3& value);

	/// <summary>
	/// Set a 4d vector uniform.
	/// </summary>
	/// <param name="name">The name of the uniform.</param>
	/// <param name="value">The value to set.</param>
	void set_uniform(const std::string_view name, const glm::vec4& value);

private:
	struct Data;
	const std::unique_ptr<Data> data;
};
