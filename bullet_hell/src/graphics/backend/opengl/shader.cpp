#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include <format>

#include "graphics/frontend/shader.h"

#include "debugging/logger.h"
#include "graphics/backend/base/uniforms_map.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"
#include "utilities/opengl_util.h"

#include "glad.h"

GLuint map_shader_type(const Shader::Type& type)
{
	switch (type)
	{
	case Shader::Type::VERTEX:
		return GL_VERTEX_SHADER;
	case Shader::Type::FRAGMENT:
		return GL_FRAGMENT_SHADER;
	case Shader::Type::COMPUTE:
		return GL_COMPUTE_SHADER;
	default:
		LOG_FATAL("Unknown shader type");
		return GL_INVALID_ENUM;
	}
}

Shader::Shader(std::vector<Module> modules)
	: handle{ glCreateProgram() }
	, uniforms{ handle }
{
	LOG_ASSERT(handle != 0 && "Failed to create a shader program");

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
		glDetachShader(handle, *it);
		glDeleteShader(*it);
	}
}

Shader::~Shader()
{
	unbind();
	if (handle != 0)
	{
		glDeleteProgram(handle);
	}
}

void Shader::bind()
{
	glUseProgram(handle);
}

void Shader::unbind()
{
	glUseProgram(0);
}

void Shader::link()
{
	glLinkProgram(handle);

	GLint link_status = GL_TRUE;
	glGetProgramiv(handle, GL_LINK_STATUS, &link_status);

	OpenGLUtil::check_shader_program_linked(handle);
	OpenGLUtil::check_gl_errors();
}

void Shader::validate()
{
	glValidateProgram(handle);

	GLint program_status = GL_TRUE;
	glGetProgramiv(handle, GL_VALIDATE_STATUS, &program_status);

	if (program_status == GL_FALSE)
	{
		GLint log_length = 0;
		char log[1024];
		glGetProgramInfoLog(handle, sizeof(log), &log_length, log);

		LOG_ERROR(log);
	}
}

ShaderModuleHandle Shader::create_shader(const Shader::Module& shader_data)
{
	GLuint shader_id = glCreateShader(map_shader_type(shader_data.type));

	LOG_ASSERT(shader_id != 0 && "Failed to create shader module");

	Resource file(shader_data.file_name);
	std::shared_ptr<ResourceHandle> shader_handle =
		g_game_logic->resource_cache->get_handle(&file);

	if (!shader_handle)
	{
		LOG_ERROR(std::format("Cannot find shader {}", shader_data.file_name));
		return false;
	}

	const char* shader_source = shader_handle->get_buffer();
	const int source_length = static_cast<int>(shader_handle->get_size());
	
	glShaderSource(shader_id, 1, &shader_source, &source_length);
	OpenGLUtil::check_gl_errors();
	glCompileShader(shader_id);

	OpenGLUtil::check_shader_compiled(shader_id);

#ifdef DEBUG
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

#endif //DEBUG
	OpenGLUtil::check_gl_errors();

	glAttachShader(handle, shader_id);

	return shader_id;
}

#endif