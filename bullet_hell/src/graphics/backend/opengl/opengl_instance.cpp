#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include <format>

#include "debugging/logger.h"
#include "graphics/frontend/buffer.h"
#include "graphics/frontend/framebuffer.h"
#include "graphics/frontend/instance.h"
#include "graphics/frontend/texture.h"
#include "graphics/gui/ui.h"
#include "memory/memory_util.h"

#include "glad.h"

//TODO(ches) BH-50 - fill this out

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
	ImGui_ImplOpenGL3_Init("#version 460");
}

Instance::~Instance()
{
	safe_delete(pipeline);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void delete_resource(DeletionQueue::Entry entry) {
	switch (entry.type) {
	case DeletionQueue::ResourceType::BUFFER:
	{
		Buffer* resource = static_cast<Buffer*>(entry.resource);
		GLuint handle = static_cast<GLuint>(resource->handle);
		glDeleteBuffers(1, &handle);
		safe_delete(resource);
	}
		break;
	case DeletionQueue::ResourceType::FRAMEBUFFER:
	{
		Framebuffer* resource = static_cast<Framebuffer*>(entry.resource);
		GLuint handle = static_cast<GLuint>(resource->handle);
		glDeleteFramebuffers(1, &handle);
		
		for (TextureHandle& texture : resource->textures) {
			GLuint gl_texture = static_cast<GLuint>(texture);
			glDeleteTextures(1, &gl_texture);
		}
		safe_delete(resource);
	}
		break;
	case DeletionQueue::ResourceType::TEXTURE:
	{
		Texture* resource = static_cast<Texture*>(entry.resource);
		GLuint handle = static_cast<GLuint>(resource->handle);
		glDeleteTextures(1, &handle);
		safe_delete(resource);
	}
		break;
	default:
		LOG_INFO("Trying to delete unknown resource type");
	}
}

void Instance::initialize(const Window& window)
{
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Instance::process_resources()
{
	std::optional<DeletionQueue::Entry> to_delete = deletion_queue.pop();
	if (to_delete) {
		delete_resource(*to_delete);
	}
}

void Instance::render(const Scene& scene)
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