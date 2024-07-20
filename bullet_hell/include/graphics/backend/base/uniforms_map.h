#pragma once

#include <memory>
#include <string_view>

#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"

struct UniformsMap
{
	void create_uniform(const std::string_view name);

	void set_uniform(const std::string_view name, float value);
	void set_uniform(const std::string_view name, int value);
	void set_uniform(const std::string_view name, glm::mat4 value);
	void set_uniform(const std::string_view name, glm::vec2 value);
	void set_uniform(const std::string_view name, glm::vec3 value);
	void set_uniform(const std::string_view name, glm::vec4 value);

private:
	struct Data;
	const std::unique_ptr<Data> data;
};
