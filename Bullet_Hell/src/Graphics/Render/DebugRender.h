#pragma once

#include "Globals.h"

#include <memory>

#include "Graphics/GladTypes.h"
#include "Graphics/Graph/CascadeShadowSlice.h"
#include "Graphics/Graph/ShaderProgram.h"
#include "Graphics/Graph/UniformsMap.h"
#include "Graphics/Scene/Scene.h"

/// <summary>
/// Handles rendering for debug lines.
/// </summary>
class DebugRender
{
public:
	DebugRender();
	DebugRender(const DebugRender&) = delete;
	DebugRender& operator=(const DebugRender&) = delete;
	~DebugRender() = default;

	/// <summary>
	/// Render the debug information.
	/// </summary>
	/// <param name="scene">The scene we are rendering.</param>
	void render(const Scene& scene);

	/// <summary>
	/// Update the debug lines based on the scene.
	/// </summary>
	/// <param name="scene">The scene we are rendering.</param>
	void update_lines(const Scene& scene);

private:
	std::unique_ptr<ShaderProgram> shader_program;
	std::unique_ptr<UniformsMap> uniforms_map;

	/// <summary>
	/// Set up uniforms for this stage.
	/// </summary>
	void create_uniforms();

	/// <summary>
	/// Update the line buffer for the shadow render frustrums.
	/// </summary>
	void update_frustrums();

	GLuint map_line_vao;
	GLuint map_line_data;
	int map_line_count;

	GLuint frustrum_vao;
	GLuint frustrum_data;
	int frustrum_line_count;
};
