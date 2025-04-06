#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <format>

#include "graphics/frontend/shader.h"

#include "debugging/logger.h"
#include "graphics/frontend/uniforms_map.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

//TODO(ches) fill this out

Shader::Shader(std::vector<Module> modules)
	: handle{ }
	, uniforms{ handle }
{
	LOG_ASSERT(handle != 0 && "Failed to create a shader program");
}

Shader::~Shader()
{
	unbind();
}

void Shader::bind()
{
}

void Shader::unbind()
{
}

void Shader::link()
{
	
}

void Shader::validate()
{
	
}

ShaderModuleHandle Shader::create_shader(const Shader::Module& shader_data)
{
	return 0;
}

#endif