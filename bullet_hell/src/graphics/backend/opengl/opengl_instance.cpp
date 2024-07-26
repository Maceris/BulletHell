#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/frontend/instance.h"

#include <format>

Instance::Instance(Window& window)
	: shader_map{}
	, pipeline_manager{ window, shader_map }
	, pipeline{ pipeline_manager.get_pipeline(RenderConfigPrefab::JUST_GUI) }
{}

Instance::~Instance()
{

}
void Instance::initialize(const Window& window)
{

}

void Instance::cleanup()
{

}

void Instance::process_resources()
{

}

void Instance::render(const Scene& scene)
{

}

void Instance::resize(int width, int height)
{

}

void Instance::setup_data(const Scene& scene)
{

}

void Instance::swap_pipeline(RenderConfig config)
{

}

void Instance::set_filter(const std::string_view shader_path)
{
	std::vector<Shader::Module> module_data;

	module_data.emplace_back(std::format("{}.frag", shader_path),
		Shader::Type::FRAGMENT);
	module_data.emplace_back(std::format("{}.vert", shader_path),
		Shader::Type::VERTEX);

	Shader* shader = ALLOC Shader(module_data);
	shader->uniforms.create_uniform("screen_texture");

	shader_map.add_shader(RenderStage::Type::FILTER, shader);
}


#endif