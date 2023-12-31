#pragma once

#include "Globals.h"

#include <memory>
#include <vector>

#include "Graphics/Graph/CascadeShadowSlice.h"
#include "Graphics/Graph/ShaderProgram.h"
#include "Graphics/Graph/ShadowBuffer.h"
#include "Graphics/Graph/UniformsMap.h"

struct CommandBuffers;
class RenderBuffers;
class Scene;

/// <summary>
/// Handles rendering for shadows.
/// </summary>
class ShadowRender
{
public:
	/// <summary>
	/// The details for each slice like the view matrix and split distances.
	/// </summary>
	std::vector<CascadeShadowSlice> cascade_shadows;
	/// <summary>
	/// The depth buffers for the shadow mapping.
	/// </summary>
	ShadowBuffer shadow_buffer;

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
