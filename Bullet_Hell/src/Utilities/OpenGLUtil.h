#pragma once

#include "Globals.h"

#include <string>
#include <vector>

#include "glad.h"
#include "glfw3.h"

/// <summary>
/// Utilities relating to OpenGL.
/// </summary>
class OpenGLUtil
{
public:
	
	/// <summary>
	/// Check for a list of required extensions, throw an error if any do 
	/// not.
	/// </summary>
	/// <returns>Whether all extensions were found.</returns>
	static bool assert_extensions_exist(const std::vector<std::string>& names);

	/// <summary>
	/// Check for all active errors, record them, then return whether we
	/// found any.
	/// </summary>
	static bool check_gl_errors();

	/// <summary>
	/// Check if a shader is compiled, and if not then print a warning and
	/// delete the shader.
	/// </summary>
	static bool check_shader_compiled(const GLuint& shader_ID);

	/// <summary>
	/// Check if a shader program is linked, and if not then print a warning
	/// and delete the shader program.
	/// </summary>
	static bool check_shader_program_linked(const GLuint& program_ID);

	/// <summary>
	/// Log all the loaded OpenGL extensions.
	/// </summary>
	static void log_extensions();

private:
	static std::vector<std::string> cached_extensions;
};