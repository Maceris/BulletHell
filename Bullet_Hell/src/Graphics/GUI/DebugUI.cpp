#include "DebugUI.h"

#include "imgui.h"

#include "GameLogic.h"

#include <format>

#pragma region Variables
bool DebugUI::show_debug_window = true;
bool DebugUI::wireframe = false;
#pragma endregion

constexpr ImVec4 RED = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);

void DebugUI::draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::Checkbox("Debug", &DebugUI::show_debug_window);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Render Controls"))
		{
			ImGui::Checkbox("Wireframe", &DebugUI::wireframe);
			ImGui::EndMenu();
		}
		ImGui::PushStyleColor(ImGuiCol_Text, RED);
		if (ImGui::MenuItem("Quit Game"))
		{
			g_game_logic->request_close();
		}
		ImGui::PopStyleColor();

		ImGui::EndMainMenuBar();
	}

	if (DebugUI::show_debug_window)
	{
		draw_window_debug();
	}
}

void DebugUI::draw_window_debug()
{
	ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_FirstUseEver);

	ImGui::Begin("Debug");
	std::shared_ptr<Scene> scene = g_game_logic->current_scene;
	
	const glm::vec3 position = scene->camera.position;
	ImGui::Text(std::format("Camera position: (%f, %f, %f)", position.x,
		position.y, position.z).c_str());

	const int models_loaded = (int) scene->model_map.size();
	ImGui::Text(std::format("Models loaded: {}", 
		std::to_string(models_loaded)).c_str());

	ImGui::End();
}

void DebugUI::handle_input()
{
	const Render* render = g_game_logic->render.get();
	if (render)
	{
		render->configuration.wireframe = DebugUI::wireframe;
	}
}