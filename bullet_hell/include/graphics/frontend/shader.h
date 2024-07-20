#pragma once

#include <cstdint>
#include <string_view>

#include "graphics/backend/base/uniforms_map.h"

using ShaderHandle = uint64_t;

struct Shader
{
	enum class Type;

	struct Module
	{
		std::string_view file_name;
		Type type;
	};

	void bind();
	void unbind();
	
	ShaderHandle handle;
	UniformsMap uniforms;
};

enum class Shader::Type : uint8_t
{
	VERTEX,
	FRAGMENT,
	COMPUTE
};
