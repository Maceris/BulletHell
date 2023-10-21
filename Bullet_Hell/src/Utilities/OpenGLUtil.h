#pragma once

#include "Globals.h"

#include "glad.h"
#include "glfw3.h"

// Utilities relating to OpenGL
class OpenGLUtil
{
public:
	/*
	Check if a shader is compiled, and if not then print a warning and delete
	the shader.
	*/
	static bool check_shader_compiled(const GLuint& shader_ID);

	/*
	Check if a shader program is linked, and if not then print a warning and
	delete the shader program.
	*/
	static bool check_shader_program_linked(const GLuint& program_ID);

	/*
	Check for all active errors, record them, then return whether we found any
	*/
	static bool check_gl_errors();
};