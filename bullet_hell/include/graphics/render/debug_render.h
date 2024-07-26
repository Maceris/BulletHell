#pragma once

#include "globals.h"

#include <memory>

#include "graphics/glad_types.h"
#include "graphics/backend/base/debug_info.h"
#include "graphics/frontend/uniforms_map.h"
#include "graphics/graph/shader_program.h"

class Scene;

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
	/// Update the lines for AABB's of entities.
	/// </summary>
	void update_AABBs(const Scene& scene);

	DebugInfo* debug_info;
};
