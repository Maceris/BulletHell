#include "Graphics/GUI/DebugUI.h"

#if DEBUG

#include <format>

#include "glm.hpp"
#include "imgui.h"

#include "Entities/Pawn.h"
#include "Entities/PawnManager.h"
#include "Debugging/Timer.h"
#include "Graphics/Graph/Animation.h"
#include "Graphics/Render/Render.h"
#include "Graphics/Scene/Entity.h"
#include "Graphics/Scene/Scene.h"
#include "Main/GameLogic.h"

#pragma region Variables
bool DebugUI::show_debug_window = true;
bool DebugUI::show_scene_window = false;
bool DebugUI::show_timing_window = false;
bool DebugUI::wireframe = false;
bool DebugUI::debug_lines = false;
#pragma endregion

constexpr ImVec4 RED = ImVec4(1.0f, 0.1f, 0.1f, 1.0f);

void DebugUI::draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::Checkbox("Debug", &DebugUI::show_debug_window);
			ImGui::Checkbox("Scene controls", &DebugUI::show_scene_window);
			ImGui::Checkbox("Timers", &DebugUI::show_timing_window);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Render Controls"))
		{
			ImGui::Checkbox("Wireframe", &DebugUI::wireframe);
			ImGui::Checkbox("Debug lines", &DebugUI::debug_lines);
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
	if (DebugUI::show_scene_window)
	{
		draw_window_scene();
	}
	if (DebugUI::show_timing_window)
	{
		draw_window_timing();
	}
}

void DebugUI::draw_window_debug()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_FirstUseEver);

	ImGui::Begin("Debug");
	std::shared_ptr<Scene> scene = g_game_logic->current_scene;
	
	const glm::vec3& camera_position = scene->camera.position;
	ImGui::Text(std::format("Camera position: ({}, {}, {})", 
		std::to_string(camera_position.x),
		std::to_string(camera_position.y),
		std::to_string(camera_position.z)).c_str());

	const glm::vec2& camera_rotation = scene->camera.rotation;
	ImGui::Text(std::format("Camera rotation: ({}, {})", 
		std::to_string(camera_rotation.x),
		std::to_string(camera_rotation.y)).c_str());

	const int models_loaded = (int) scene->model_map.size();
	ImGui::Text(std::format("Models loaded: {}", 
		std::to_string(models_loaded)).c_str());

	const long long fps = 1000000 / LAST_TIME("Last Frame");
	ImGui::Text(std::format("FPS for last frame: {}", 
		std::to_string(fps)).c_str());

	const long long actual_fps = g_game_logic->last_FPS;
	ImGui::Text(std::format("FPS for the last second: {}", 
		std::to_string(actual_fps)).c_str());

	const glm::vec3& player_position = 
		g_pawn_manager->player->scene_entity->position;
	ImGui::Text(std::format("Player position: ({}, {}, {})",
		std::to_string(player_position.x),
		std::to_string(player_position.y),
		std::to_string(player_position.z)).c_str());

	const glm::vec2& desired_player_rotation =
		g_pawn_manager->player->desired_facing;
	ImGui::Text(std::format("Desired player rotation: ({}, {})",
		std::to_string(desired_player_rotation.x),
		std::to_string(desired_player_rotation.y)).c_str());

	const glm::quat& player_rotation = 
		g_pawn_manager->player->scene_entity->rotation;
	ImGui::Text(std::format("Actual player rotation: ({}, {}, {}, {})",
		std::to_string(player_rotation.x),
		std::to_string(player_rotation.y),
		std::to_string(player_rotation.z), 
		std::to_string(player_rotation.w)).c_str());

	const auto& animation_data = 
		g_pawn_manager->player->scene_entity->animation_data;
	ImGui::Text(std::format("Player animation: {}",
		animation_data.current_animation != nullptr 
		? animation_data.current_animation->name :"(none)").c_str());
	ImGui::Text(std::format("Player animation frame: {}",
		std::to_string(animation_data.current_frame_index)).c_str());

	const size_t enemy_count = g_pawn_manager->enemies.size();
	ImGui::Text(std::format("Enemy count: {}",
		std::to_string(enemy_count)).c_str());
	
	const size_t enemy_bullet_count = g_pawn_manager->enemy_bullets.size();
	ImGui::Text(std::format("Enemy bullet count: {}",
		std::to_string(enemy_bullet_count)).c_str());

	const size_t player_bullet_count = g_pawn_manager->player_bullets.size();
	ImGui::Text(std::format("Player bullet count: {}",
		std::to_string(player_bullet_count)).c_str());

	ImGui::End();
}

void DebugUI::draw_window_scene()
{
	ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_FirstUseEver);

	ImGui::Begin("Scene Controls");

	ImGui::Text("Controls WIP, just showing current state");

	std::shared_ptr<Scene> scene = g_game_logic->current_scene;

	if (ImGui::TreeNode("Ambient Light"))
	{
		const auto& light = scene->scene_lights.ambient_light;
		ImGui::Text(std::format("Ambient light intensity {}", 
			std::to_string(light.intensity))
			.c_str()
		);
		ImGui::Text(std::format("Ambient light color r:{} g:{} b:{}",
			std::to_string(light.color.r),
			std::to_string(light.color.g),
			std::to_string(light.color.b)).c_str()
		);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Directional Light"))
	{
		const auto& light = scene->scene_lights.directional_light;
		ImGui::Text(std::format("Directional light intensity {}",
			std::to_string(light.intensity))
			.c_str()
		);
		ImGui::Text(std::format("Directional light color r:{} g:{} b:{}",
			std::to_string(light.color.r),
			std::to_string(light.color.g),
			std::to_string(light.color.b)).c_str()
		);
		ImGui::Text(std::format("Directional light angle x:{} y:{} z:{}",
			std::to_string(light.direction.x),
			std::to_string(light.direction.y),
			std::to_string(light.direction.z)).c_str()
		);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Fog"))
	{
		const auto& fog = scene->fog;
		ImGui::Text(std::format("Fog enabled? {}",
			fog.active ? "yes" : "no").c_str()
		);
		ImGui::Text(std::format("Fog color r:{} g:{} b:{}",
			std::to_string(fog.color.r),
			std::to_string(fog.color.g),
			std::to_string(fog.color.b)).c_str()
		);
		ImGui::Text(std::format("Fog density {}",
			std::to_string(fog.density)).c_str()
		);
		ImGui::TreePop();
	}
	
	ImGui::End();
}

void DebugUI::draw_window_timing()
{
	ImGui::SetNextWindowPos(ImVec2(460, 30), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(450, 400), ImGuiCond_FirstUseEver);

	ImGui::Begin("Timers (Average)");
	
	if (ImGui::Button("Clear all timers"))
	{
		CLEAR_TIMER_HISTORY();
	}

	for (auto& stage : TIME_STAGES_LIST)
	{
		ImGui::Text(std::format("{}: {} microseconds", stage, 
			std::to_string(AVERAGE_TIME(stage))).c_str());
	}

	ImGui::End();
}

void DebugUI::handle_input()
{
	const Render* render = g_game_logic->render.get();
	if (render)
	{
		render->configuration.wireframe = DebugUI::wireframe;
		render->configuration.debug_lines = DebugUI::debug_lines;
	}
}
#else
void DebugUI::draw() {}
void DebugUI::draw_window_debug() {}
void DebugUI::draw_window_scene() {}
void DebugUI::draw_window_timing() {}
void DebugUI::handle_input() {}
#endif