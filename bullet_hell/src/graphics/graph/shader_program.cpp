#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/graph/shader_program.h"

#include "debugging/logger.h"
#include "utilities/opengl_util.h"

#include "glad.h"

ShaderModuleData::ShaderModuleData(const char* source, int length, 
	const GLuint type)
	: source_code{ source }
	, source_length{ length }
	, shader_type{ type }
{}

ShaderProgram::ShaderProgram(std::vector<ShaderModuleData> modules)
	: program_id(glCreateProgram())
{
	LOG_ASSERT(program_id != 0 && "Failed to create a shader program");

	OpenGLUtil::check_gl_errors();

	std::vector<GLuint> shader_modules;

	for (auto it = modules.begin(); it != modules.end(); ++it)
	{
		shader_modules.push_back(create_shader(*it));
	}

	link();
	validate();

	for (auto it = shader_modules.begin(); it != shader_modules.end(); ++it)
	{
		glDetachShader(program_id, *it);
		glDeleteShader(*it);
	}
}

ShaderProgram::~ShaderProgram()
{
	unbind();
	if (program_id != 0)
	{
		glDeleteProgram(program_id);
	}
}

GLuint ShaderProgram::create_shader(const ShaderModuleData& shader_data)
{
	GLuint shader_id = glCreateShader(shader_data.shader_type);

	LOG_ASSERT(shader_id != 0 && "Failed to create shader module");

	glShaderSource(shader_id, 1, &shader_data.source_code,
		&shader_data.source_length);
	OpenGLUtil::check_gl_errors();
	glCompileShader(shader_id);

	OpenGLUtil::check_shader_compiled(shader_id);

#ifdef _DEBUG
	GLint max_length = 0;
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);

	if (max_length > 0)
	{
		std::string log;
		log.reserve(max_length);

		glGetShaderInfoLog(shader_id, max_length, &max_length, &log[0]);

		LOG_INFO("Shader log for shader " + std::to_string(shader_id) 
			+ ": " + log);
	}
	
#endif //_DEBUG
	OpenGLUtil::check_gl_errors();

	glAttachShader(program_id, shader_id);

	return shader_id;
}

void ShaderProgram::link()
{
	glLinkProgram(program_id);

	GLint link_status = GL_TRUE;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

	OpenGLUtil::check_shader_program_linked(program_id);
	OpenGLUtil::check_gl_errors();
}

void ShaderProgram::validate()
{
	glValidateProgram(program_id);

	GLint program_status = GL_TRUE;
	glGetProgramiv(program_id, GL_VALIDATE_STATUS, &program_status);

	if (program_status == GL_FALSE)
	{
		GLint log_length = 0;
		char log[1024];
		glGetProgramInfoLog(program_id, sizeof(log), &log_length, log);

		LOG_ERROR(log);
	}
}

void ShaderProgram::bind()
{
	glUseProgram(program_id);
}

void ShaderProgram::unbind()
{
	glUseProgram(0);
}

#endif
