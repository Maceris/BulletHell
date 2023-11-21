#include "Graphics/GUI/UI.h"

#include "imgui.h"

#if DEBUG
#include "Graphics/GUI/DebugUI.h"
#endif
#include "Entities/PawnManager.h"
#include "Main/GameLogic.h"

void UI::draw()
{
#if DEBUG
	DebugUI::draw();
#endif
	UI::draw_player_health();
}

void UI::handle_input()
{
#if DEBUG
	DebugUI::handle_input();
#endif
}

void UI::draw_player_health()
{

	ImGui::SetNextWindowSize(ImVec2(300, 30), ImGuiCond_FirstUseEver);
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(
		ImVec2(io.DisplaySize.x - 310, 30), ImGuiCond_FirstUseEver);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	bool is_open = true;
	ImGui::Begin("Player Health", &is_open, 
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs
	);
	
	const int current_health = g_pawn_manager->player->health;
	const int max_health = g_pawn_manager->player->max_health;
	const float health = static_cast<float>(current_health) / max_health;

	std::string overlay = std::to_string(current_health) + "/" 
		+ std::to_string(max_health);
	
	ImGui::ProgressBar(health, ImVec2(300, 30), overlay.c_str());

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();

}