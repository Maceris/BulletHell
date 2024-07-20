#pragma once

#include <map>
#include <memory>

#include "graphics/frontend/shader.h"
#include "graphics/backend/base/render_stage.h"

struct ShaderMap
{
	void add_shader(RenderStage::Type type, std::shared_ptr<Shader> shader);
	void clear_all();
	void remove_shader(RenderStage::Type type);
	std::shared_ptr<Shader> get_shader(RenderStage::Type type);

	std::map<RenderStage::Type, std::shared_ptr<Shader>> shaders;
};