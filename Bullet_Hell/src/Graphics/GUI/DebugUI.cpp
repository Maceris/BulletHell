#include "DebugUI.h"

#include "imgui.h"

#include "GameLogic.h"
#include "Timer.h"

#include <format>

#pragma region Variables
bool DebugUI::show_debug_window = true;
bool DebugUI::show_scene_window = false;
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
			ImGui::Checkbox("Scene controls", &DebugUI::show_scene_window);
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
	ImGui::Text(std::format("FPS: {}", std::to_string(fps)).c_str());

	const glm::vec3& player_position = scene->player->position;
	ImGui::Text(std::format("Player position: ({}, {}, {})",
		std::to_string(player_position.x),
		std::to_string(player_position.y),
		std::to_string(player_position.z)).c_str());

	const glm::quat& player_rotation = scene->player->rotation;
	ImGui::Text(std::format("Player rotation: ({}, {})",
		std::to_string(player_rotation.x),
		std::to_string(player_rotation.y)).c_str());

	const auto& animation_data = scene->player->animation_data;
	ImGui::Text(std::format("Player animation: {}",
		animation_data.current_animation != nullptr 
		? animation_data.current_animation->name :"(none)").c_str());
	ImGui::Text(std::format("Player animation frame: {}",
		std::to_string(animation_data.current_frame_index)).c_str());

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