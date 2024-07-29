#pragma once

#include "graphics/backend/base/debug_info.h"
#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

class DebugRender : RenderStage
{
public:
	DebugRender(DebugInfo* debug_info)
		: debug_info{ debug_info }
	{
		//TODO(ches) set up shader
	}

	void render(Scene& scene);

private:
	Shader* shader;
	DebugInfo* const debug_info;

	/// <summary>
	/// Update the lines for AABB's of entities.
	/// </summary>
	void update_AABBs(const Scene& scene);

	/// <summary>
	/// Update the debug lines based on the scene.
	/// </summary>
	/// <param name="scene">The scene we are rendering.</param>
	void update_lines(const Scene& scene);
};
