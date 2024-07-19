#include "graphics/render/gui_render.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include "debugging/logger.h"
#include "graphics/window.h"
#include "graphics/scene/scene.h"
#include "graphics/gui/ui.h"
#include "main/game_logic.h"

#include "glad.h"

#pragma region Shader code

const char fragment_shader_source[] = R"glsl(
#version 460

in vec2 texture_coordinates;
in vec4 fragment_color;

uniform sampler2D texture_sampler;

out vec4 color_out;

void main()
{
    color_out = fragment_color  * texture(texture_sampler, texture_coordinates);
}
)glsl";

const char vertex_shader_source[] = R"glsl(
#version 460

layout (location=0) in vec2 position_in;
layout (location=1) in vec2 texture_coordinates_in;
layout (location=2) in vec4 color_in;

out vec2 texture_coordinates;
out vec4 fragment_color;

uniform vec2 scale;

void main()
{
    texture_coordinates = texture_coordinates_in;
    fragment_color = color_in;
    gl_Position = vec4(position_in * scale + vec2(-1.0, 1.0), 0.0, 1.0);
}
)glsl";
#pragma endregion

GuiRender::GuiRender(const Window& window)
	: scale{ window.width, window.height }
{
	std::vector<ShaderModuleData> shader_modules;
	shader_modules.emplace_back(vertex_shader_source,
		sizeof(vertex_shader_source), GL_VERTEX_SHADER);
	shader_modules.emplace_back(fragment_shader_source, 
		sizeof(fragment_shader_source), GL_FRAGMENT_SHADER);
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
	case GAME_OVER:
		UI::draw_game_over();
		break;
	case MENU:
		UI::draw_main_menu();
		break;
	case PAUSED:
		UI::draw_pause_menu();
		break;
	case RUNNING:
		UI::draw_in_game();
		break;
	case STARTING_UP:
	case QUIT_REQUESTED:
	case QUITTING:
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
