#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL_DEPRECATED

#include "graphics/render/gui_render.h"

#include "debugging/logger.h"
#include "graphics/window.h"
#include "graphics/scene/scene.h"
#include "graphics/gui/ui.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

#include "glad.h"

GuiRender::GuiRender(const Window& window)
	: scale{ window.width, window.height }
{
	Resource frag("shaders/gui.frag");
	std::shared_ptr<ResourceHandle> frag_handle =
		g_game_logic->resource_cache->get_handle(&frag);
	LOG_ASSERT(frag_handle && "Cannot find GUI fragment shader");

	Resource vert("shaders/gui.vert");
	std::shared_ptr<ResourceHandle> vert_handle =
		g_game_logic->resource_cache->get_handle(&vert);
	LOG_ASSERT(vert_handle && "Cannot find GUI vertex shader");

	std::vector<ShaderModuleData> shader_modules;
	shader_modules.emplace_back(vert_handle->get_buffer(),
		vert_handle->get_size(), GL_VERTEX_SHADER);
	shader_modules.emplace_back(frag_handle->get_buffer(),
		frag_handle->get_size(), GL_FRAGMENT_SHADER);
	shader_program = std::make_unique<ShaderProgram>(shader_modules);

	create_uniforms();
	create_ui_resources(window);
}

void GuiRender::create_ui_resources(const Window& window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window.handle, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}

void GuiRender::create_uniforms()
{
	uniforms_map = std::make_unique<UniformsMap>(shader_program->program_id);
	uniforms_map->create_uniform("scale");
}

void GuiRender::render(const Scene& scene)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	switch (g_game_logic->get_current_state())
	{
	case GameState::GAME_OVER:
		UI::draw_game_over();
		break;
	case GameState::MENU:
		UI::draw_main_menu();
		break;
	case GameState::PAUSED:
		UI::draw_pause_menu();
		break;
	case GameState::RUNNING:
		UI::draw_in_game();
		break;
	case GameState::STARTING_UP:
	case GameState::QUIT_REQUESTED:
	case GameState::QUITTING:
	default:
		LOG_ERROR("Rending GUI during an invalid render state");
		break;
	}
	
	UI::handle_input();
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiRender::resize(const unsigned int width, const unsigned int height)
{
	ImGuiIO& imGuiIO = ImGui::GetIO();
	imGuiIO.DisplaySize.x = static_cast<float>(width);
	imGuiIO.DisplaySize.y = static_cast<float>(height);
}

GuiRender::~GuiRender()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
#endif
