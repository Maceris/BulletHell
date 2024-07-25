#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "globals.h"
#include "graphics/backend/base/shader_map.h"

ShaderMap::ShaderMap()
	: shaders{}
{}

ShaderMap::~ShaderMap()
{
	clear_all();
}

void ShaderMap::add_shader(RenderStage::Type type, Shader* shader)
{
	remove_shader(type);
	
}

void ShaderMap::clear_all()
{
	for (RenderStage::Type type : RenderStageTypeIterator())
	{
		remove_shader(type);
	}
}

void ShaderMap::remove_shader(RenderStage::Type type)
{
	auto result = shaders.find(type);
	if (result != shaders.end())
	{
		Shader* existing = result->second;
		shaders.erase(result);
		safe_delete(existing);
	}
}

Shader* ShaderMap::get_shader(RenderStage::Type type)
{
	auto result = shaders.find(type);
	if (result != shaders.end())
	{
		return result->second;
	}
	return nullptr;
}

#endif