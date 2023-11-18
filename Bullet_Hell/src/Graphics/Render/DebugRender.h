#pragma once

#include "Globals.h"

#include <memory>

#include "Graphics/GladTypes.h"
#include "Graphics/Graph/CascadeShadowSlice.h"
#include "Graphics/Graph/ShaderProgram.h"
#include "Graphics/Graph/UniformsMap.h"
#include "Graphics/Scene/Scene.h"

/// <summary>
/// A set of lines to draw.
/// </summary>
struct LineGroup
{
	/// <summary>
	/// The VAO for this group.
	/// </summary>
	GLuint vao;
	/// <summary>
	/// The buffer handle for the line data.
	/// </summary>
	GLuint data;
	/// <summary>
	/// The number of lines. Notably, not the number of points like 
	/// glDrawArrays expects.
	/// </summary>
	int count;

	LineGroup();
	LineGroup(const LineGroup&) = delete;
	LineGroup& operator=(const LineGroup&) = delete;
	~LineGroup();
};

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
	/// Update the line buffer for the shadow render frustums.
	/// </summary>
	void update_frustums();

	LineGroup map_lines;
	LineGroup frustum_lines;
	LineGroup AABB_lines;
};
