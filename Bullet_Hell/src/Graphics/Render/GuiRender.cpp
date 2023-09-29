#include "../../Utilities/Globals.h"
#include "GuiRender.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#pragma region Shader code

const char fragment_shader_source[] = R"glsl(
#version 460

in vec2 texture_coordinates;
in vec4 color_in;

uniform sampler2D texture_sampler;

out vec4 color_out;

void main()
{
    color_out = color_in  * texture(texture_sampler, texture_coordinates);
}
)glsl";

const char vertex_shader_source[] = R"glsl(
#version 460

layout (location=0) in vec2 position_in;
layout (location=1) in vec2 texture_coordinates_in;
layout (location=2) in vec4 color_in;

out vec2 texture_coordinates_out;
out vec4 color_out;

uniform vec2 scale;

void main()
{
    texture_coordinates_out = texture_coordinates_in;
    color_out = color_in;
    gl_Position = vec4(position_in * scale + vec2(-1.0, 1.0), 0.0, 1.0);
}
)glsl";
#pragma endregion

GuiRender::GuiRender(const Window& window)
	: scale(glm::vec2(window.width, window.height))
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
	ImGui_ImplOpenGL3_Init("#version 130");
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
	ImGui::ShowDemoWindow();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiRender::resize(const unsigned int width, const unsigned int height)
{
	//TODO(ches) resize
}

void GuiRender::setup_imgui_keys()
{
	//TODO(ches) setup keys
}

GuiRender::~GuiRender()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}