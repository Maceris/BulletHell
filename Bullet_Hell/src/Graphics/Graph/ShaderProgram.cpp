#include "ShaderProgram.h"

#include "Logger.h"

ShaderModuleData::ShaderModuleData(const char* source, int length, 
	const GLuint type)
	: source_code(source)
	, source_length(length)
	, shader_type(type)
{}

ShaderProgram::ShaderProgram(std::vector<ShaderModuleData> modules)
	: program_id(glCreateProgram())
{
	if (program_id == 0)
	{
		LOG_WARNING("Failed to create a shader program");
	}

	std::vector<GLuint> shader_modules;

	for (auto it = modules.begin(); it != modules.end(); ++it)
	{
		shader_modules.push_back(create_shader(*it));
	}

	link();

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

	if (shader_id == 0)
	{
		LOG_WARNING("Failed to create shader module");
	}

	glShaderSource(program_id, 1, &shader_data.source_code, 
		&shader_data.source_length);
	glCompileShader(shader_id);

	GLint compile_status = GL_TRUE;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);

	if (compile_status == GL_FALSE)
	{
		GLint log_length = 0;
		char log[1024];
		glGetShaderInfoLog(shader_id, sizeof(log), &log_length, log);

		LOG_WARNING(log);
	}

	glAttachShader(program_id, shader_id);

	return shader_id;
}

void ShaderProgram::link()
{
	glLinkProgram(program_id);

	GLint link_status = GL_TRUE;
	glGetProgramiv(program_id, GL_LINK_STATUS, &link_status);

	if (link_status == GL_FALSE)
	{
		GLint log_length = 0;
		char log[1024];
		glGetProgramInfoLog(program_id, sizeof(log), &log_length, log);

		LOG_WARNING(log);
	}
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

		LOG_WARNING(log);
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