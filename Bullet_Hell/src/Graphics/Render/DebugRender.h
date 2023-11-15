#pragma once

#include "Globals.h"

#include <memory>

#include "Graphics/GladTypes.h"
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

	void update_lines(const Scene& scene);

private:
	std::unique_ptr<ShaderProgram> shader_program;
	std::unique_ptr<UniformsMap> uniforms_map;

	int line_count;

	/// <summary>
	/// Set up uniforms for this stage.
	/// </summary>
	void create_uniforms();

	GLuint line_vao;
	GLuint line_data;
};
