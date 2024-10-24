#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_OPENGL

#include "graphics/backend/opengl/stages/gui_render.h"

#include "debugging/logger.h"
#include "graphics/window.h"
#include "graphics/scene/scene.h"
#include "graphics/gui/ui.h"
#include "main/game_logic.h"
#include "memory/memory_util.h"

#include "glad.h"

GuiRender::GuiRender(const Window& window, StageResource<GuiMesh>* gui_mesh)
	: gui_mesh{ gui_mesh }
	, scale {0, 0}
{
	std::vector<Shader::Module> shader_modules;
	shader_modules.emplace_back("shaders/gui.frag",
		Shader::Type::FRAGMENT);
	shader_modules.emplace_back("shaders/gui.vert",
		Shader::Type::VERTEX);

	shader = ALLOC Shader(shader_modules);

	shader->uniforms.create_uniform("scale");
}

void GuiRender::render(Scene& scene)
{
	render_gui(scene);
}

void GuiRender::render_gui(const Scene& scene)
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

void GuiRenderStandalone::render(Scene& scene)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	render_gui(scene);
}

#endif
