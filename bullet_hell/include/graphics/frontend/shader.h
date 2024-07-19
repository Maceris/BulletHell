#pragma once

#include <cstdint>
#include <string_view>

struct Shader
{
	enum class Type;

	struct Module
	{
		std::string_view file_name;
		Type type;
	};

	void bind();

};

enum class Shader::Type : uint8_t
{
	VERTEX,
	FRAGMENT,
	COMPUTE
};
