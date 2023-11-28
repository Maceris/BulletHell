#include "Graphics/GUI/UI.h"

#include "imgui.h"

#if DEBUG
#include "Graphics/GUI/DebugUI.h"
#endif
#include "Entities/PawnManager.h"
#include "Main/GameLogic.h"
#include "ResourceCache/ResourceCache.h"

static ImFont* font_default;
static ImFont* font_noirden;

void UI::draw()
{
#if DEBUG
	DebugUI::draw();
#endif
	UI::draw_player_health();
	UI::draw_round_timer();
}

void UI::first_time_setup()
{
	Resource resource("fonts/noirden-regular.ttf");
	auto handle = g_game_logic->resource_cache->get_handle(&resource);

	ImGuiIO& io = ImGui::GetIO();
	font_default = io.Fonts->AddFontDefault();

	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;
	void* raw_data = static_cast<void*>(handle->get_writeable_buffer());
	font_noirden = io.Fonts->AddFontFromMemoryTTF(raw_data, handle->get_size(),
		32.0f, &font_cfg);
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

void UI::draw_round_timer()
{
	ImGui::SetNextWindowSize(ImVec2(100, 30), ImGuiCond_FirstUseEver);
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(
		ImVec2(io.DisplaySize.x / 2 - 50, 30), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowBgAlpha(0.1f);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	bool is_open = true;
	ImGui::Begin("Round Timer", &is_open,
		ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs
	);

	const double time = g_game_logic->round_timer;
	const int minutes = static_cast<int>(std::floor(time / 60.0));
	const double seconds = std::fmod(time, 60.0);

	const std::string timer_text = std::format("{}:{:.1f}", minutes, seconds);

	ImGui::PushFont(font_noirden);

	const float text_width = ImGui::CalcTextSize(timer_text.c_str()).x;
	const float window_width = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
	ImGui::Text(timer_text.c_str());
	
	ImGui::PopFont();

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}