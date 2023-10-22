#include "DebugUI.h"

#include "imgui.h"

#include "GameLogic.h"
#include "Timer.h"

#include <format>

#pragma region Variables
bool DebugUI::show_debug_window = true;
bool DebugUI::show_timing_window = false;
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
			ImGui::Checkbox("Timers", &DebugUI::show_timing_window);
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
	if (DebugUI::show_timing_window)
	{
		draw_window_timing();
	}
}

void DebugUI::draw_window_debug()
{
	ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_FirstUseEver);

	ImGui::Begin("Debug");
	std::shared_ptr<Scene> scene = g_game_logic->current_scene;
	
	const glm::vec3& position = scene->camera.position;
	ImGui::Text(std::format("Camera position: ({}, {}, {})", 
		std::to_string(position.x),
		std::to_string(position.y),
		std::to_string(position.z)).c_str());

	const glm::vec2& rotation = scene->camera.rotation;
	ImGui::Text(std::format("Camera rotation: ({}, {})", 
		std::to_string(rotation.x),
		std::to_string(rotation.y)).c_str());

	const int models_loaded = (int) scene->model_map.size();
	ImGui::Text(std::format("Models loaded: {}", 
		std::to_string(models_loaded)).c_str());

	const long long fps = 1000000 / LAST_TIME("Last Frame");
	ImGui::Text(std::format("FPS: {}", std::to_string(fps)).c_str());

	ImGui::End();
}

void DebugUI::draw_window_timing()
{
	ImGui::SetNextWindowPos(ImVec2(460, 30), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_FirstUseEver);

	ImGui::Begin("Timers");
	
	for (auto& stage : TIME_STAGES_LIST)
	{
		ImGui::Text(std::format("Stage {}: {} microseconds", stage, 
			std::to_string(LAST_TIME(stage))).c_str());
	}

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