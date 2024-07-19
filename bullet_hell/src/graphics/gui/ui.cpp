#include "graphics/gui/ui.h"

#include "imgui.h"

#if DEBUG
#include "graphics/gui/debug_ui.h"
#endif
#include "entities/pawn_manager.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

static ImFont* font_default;
static ImFont* font_noirden;

void draw_centered_text(const char* text)
{
	const float text_width = ImGui::CalcTextSize(text).x;
	const float window_width = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
	ImGui::Text(text);
}

bool draw_centered_button(const char* button_text)
{
	const float text_width = ImGui::CalcTextSize(button_text).x;
	const float window_width = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
	return ImGui::Button(button_text);
}

void UI::draw_game_over()
{
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(
		ImVec2(io.DisplaySize.x / 2 - 150, io.DisplaySize.y / 2 - 150),
		ImGuiCond_FirstUseEver
	);

	bool is_open = true;
	ImGui::Begin("Game Over", &is_open,
		ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoBackground
	);

	ImGui::PushFont(font_noirden);

	draw_centered_text("Game Over!");

	if (draw_centered_button("Restart Game"))
	{
		g_game_logic->reset();
	}

	if (draw_centered_button("Quit Game"))
	{
		g_game_logic->request_close();
	}

	ImGui::PopFont();

	ImGui::End();
}

void UI::draw_in_game()
{
#if DEBUG
	DebugUI::draw();
#endif
	UI::draw_player_health();
	UI::draw_round_timer();
}

void UI::draw_main_menu()
{
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(
		ImVec2(io.DisplaySize.x / 2 - 150, io.DisplaySize.y / 2 - 150),
		ImGuiCond_FirstUseEver
	);

	bool is_open = true;
	ImGui::Begin("Main Menu", &is_open,
		ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoBackground
	);

	ImGui::PushFont(font_noirden);

	draw_centered_text("Bullet Hell");

	if (draw_centered_button("Start Game"))
	{
		g_game_logic->reset();
	}

	if (draw_centered_button("Quit Game"))
	{
		g_game_logic->request_close();
	}

	ImGui::PopFont();

	ImGui::End();

	UI::draw_controls();
}

void UI::draw_pause_menu()
{
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(
		ImVec2(io.DisplaySize.x / 2 - 150, io.DisplaySize.y / 2 - 150),
		ImGuiCond_FirstUseEver
	);

	bool is_open = true;
	ImGui::Begin("Pause Menu", &is_open,
		ImGuiWindowFlags_NoDecoration 
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoBackground
	);

	ImGui::PushFont(font_noirden);

	draw_centered_text("Game Paused");

	if (draw_centered_button("Resume"))
	{
		g_game_logic->on_resume();
	}

	if (draw_centered_button("Quit Game"))
	{
		g_game_logic->request_close();
	}

	ImGui::PopFont();

	ImGui::End();

	UI::draw_controls();
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
	font_noirden = io.Fonts->AddFontFromMemoryTTF(raw_data, 
		static_cast<int>(handle->get_size()),
		32.0f, &font_cfg);
}

void UI::handle_input()
{
#if DEBUG
	DebugUI::handle_input();
#endif
}

void UI::draw_controls()
{
	ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
	ImGuiIO& io = ImGui::GetIO();
	ImGui::SetNextWindowPos(
		ImVec2(10, io.DisplaySize.y - 210), ImGuiCond_FirstUseEver);

	bool is_open = true;
	ImGui::Begin("Controls", &is_open,
		ImGuiWindowFlags_NoDecoration
		| ImGuiWindowFlags_NoInputs
		| ImGuiWindowFlags_NoBackground
	);

	ImGui::Text("Controls");
	ImGui::Text("--------");
	ImGui::Text("W,A,S,D - Move Player");
	ImGui::Text("Space - Attack");

#if DEBUG
	ImGui::NewLine();
	ImGui::Text("Debug Controls");
	ImGui::Text("--------------");
	ImGui::Text("Arrow Keys - Move Camera");
	ImGui::Text("Right Click (hold, move mouse) - Rotate Camera");
	ImGui::Text("Left Shift - Move Camera down");
	ImGui::Text("Z - Move Camera Up");
#endif

	ImGui::End();
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

	draw_centered_text(timer_text.c_str());
	
	ImGui::PopFont();

	ImGui::End();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}