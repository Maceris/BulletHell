#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <format>

#include "debugging/logger.h"
#include "graphics/frontend/buffer.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/instance.h"
#include "graphics/frontend/texture.h"
#include "graphics/gui/ui.h"
#include "memory/memory_util.h"

#include "GLFW/glfw3.h"
#include "backends/imgui_impl_glfw.h"

Configuration Instance::configuration;

Instance::Instance(Window& window)
	: deletion_queue{}
	, shader_map{}
	, pipeline_manager{ window, &deletion_queue, shader_map }
	, pipeline{ pipeline_manager.get_pipeline(RenderConfigPrefab::JUST_GUI) }
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window.handle, true);
}

Instance::~Instance()
{
	safe_delete(pipeline);

	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void delete_resource(DeletionQueue::Entry entry) {
	
}

void Instance::initialize(const Window& window)
{
	
}

void Instance::process_resources()
{
	
}

void Instance::render(Scene& scene)
{
	pipeline->render(scene, shader_map);
}

void Instance::resize(int width, int height)
{
	pipeline_manager.resize(width, height);
	ImVec2& display_size = ImGui::GetMainViewport()->Size;
	display_size.x = width;
	display_size.y = height;
}

void Instance::setup_data(Scene& scene)
{
	pipeline_manager.setup_data(scene);
}

void Instance::swap_pipeline(RenderConfig config)
{
	pipeline = pipeline_manager.get_pipeline(config);
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