#pragma once

#include "Globals.h"

#include "CascadeShadowSlice.h"
#include "CommandBuffers.h"
#include "RenderBuffers.h"
#include "ShaderProgram.h"
#include "ShadowBuffer.h"
#include "UniformsMap.h"

#include <memory>

#include "glad.h"

/// <summary>
/// Handles rendering for shadows.
/// </summary>
class ShadowRender
{
public:
	/// <summary>
	/// The details for each slice like the view matrix and split distances.
	/// </summary>
	CascadeShadowSlice cascade_shadows[SHADOW_MAP_CASCADE_COUNT];
	/// <summary>
	/// The depth buffers for the shadow mapping.
	/// </summary>
	std::unique_ptr<ShadowBuffer> shadow_buffer;

	ShadowRender();
	ShadowRender(const ShadowRender&) = delete;
	ShadowRender& operator=(const ShadowRender&) = delete;
	~ShadowRender() = default;

	/// <summary>
	/// Render the shadows for a scene.
	/// </summary>
	/// <param name="scene">The scene to render.</param>
	/// <param name="render_buffers">Bufferse for indirect drawing of models.
	/// </param>
	/// <param name="command_buffers">The rendering command buffers.</param>
	void render(const Scene& scene, const RenderBuffers& render_buffers,
		const CommandBuffers& command_buffers);
private:
	std::unique_ptr<ShaderProgram> shader_program;
	std::unique_ptr<UniformsMap> uniforms_map;
};
