#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

#include "graphics/backend/base/uniforms_map.h"

using ShaderHandle = uint64_t;
using ShaderModuleHandle = uint64_t;

struct Shader
{
	enum class Type : uint8_t;

	struct Module
	{
		std::string_view file_name;
		Type type;
	};

	/// <summary>
	/// Create a new shader program from modules.
	/// </summary>
	/// <param name="modules">The parts that make up the shader program.
	/// </param>
	Shader(std::vector<Module> modules);
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;

	/// <summary>
	/// Unbind and then delete the program.
	/// </summary>
	~Shader();

	/// <summary>
	/// Install this program as part of the current rendering state.
	/// </summary>
	void bind();

	/// <summary>
	/// Stop using the program in thte current rendering state.
	/// </summary>
	void unbind();
	
	ShaderHandle handle;
	UniformsMap uniforms;
private:
	/// <summary>
	/// Create a shader from file.
	/// </summary>
	/// <param name="shader_data">The information about the module.</param>
	ShaderModuleHandle create_shader(const Shader::Module& shader_data);

	/// <summary>
	/// Link a series of shader modules that make up the total shader program.
	/// </summary>
	/// <param name="shader_modules">The modules to link up.</param>
	void link();

	/// <summary>
	/// Validate the program after compiling.
	/// </summary>
	void validate();
};

enum class Shader::Type : uint8_t
{
	VERTEX,
	FRAGMENT,
	COMPUTE
};
