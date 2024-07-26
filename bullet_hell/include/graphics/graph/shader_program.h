#pragma once

#include <string>
#include <vector>

#include "graphics/glad_types.h"

/// <summary>
/// Details about a shader.
/// </summary>
struct ShaderModuleData
{
	/// <summary>
	/// The source code of the shader.
	/// </summary>
	const char* source_code;

	/// <summary>
	/// The number of characters in the source code.
	/// </summary>
	const int source_length;

	/// <summary>
	/// One of:
	/// GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER, 
	/// GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER
	/// </summary>
	const GLuint shader_type;

	/// <summary>
	/// Specify a resource and type.
	/// </summary>
	/// <param name="name">The source code of the shader.</param>
	/// <param name="length">The length of the source code.</param>
	/// <param name="type">The type of shader.</param>
	ShaderModuleData(const char* source, int length, 
		const GLuint type);

	ShaderModuleData(const ShaderModuleData&) = default;
	ShaderModuleData& operator=(const ShaderModuleData&) = default;
	~ShaderModuleData() = default;
};

class ShaderProgram
{
private:
	/// <summary>
	/// Create a shader from code.
	/// </summary>
	/// <param name="shader_data">The information about the shader.</param>
	GLuint create_shader(const ShaderModuleData& shader_data);

	/// <summary>
	/// Link a series of shader modules that make up the total shader program.
	/// </summary>
	/// <param name="shader_modules">The modules to link up.</param>
	void link();

	/// <summary>
	/// Validate the program after compiling.
	/// </summary>
	void validate();

public:
	/// <summary>
	/// The ID of this shader program.
	/// </summary>
	const GLuint program_id;

	/// <summary>
	/// Create a new shader program from modules.
	/// </summary>
	/// <param name="modules">The parts that make up the shader program.
	/// </param>
	ShaderProgram(std::vector<ShaderModuleData> modules);

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

	/// <summary>
	/// Unbind and then delete the program.
	/// </summary>
	~ShaderProgram();

	/// <summary>
	/// Install this program as part of the current rendering state.
	/// </summary>
	void bind();

	/// <summary>
	/// Stop using the program in thte current rendering state.
	/// </summary>
	void unbind();
};