#pragma once

#include "Globals.h"

#include <memory>

#include "Graphics/Graph/GBuffer.h"
#include "Graphics/Graph/RenderBuffers.h"
#include "Graphics/Graph/ShaderProgram.h"
#include "Graphics/Graph/UniformsMap.h"
#include "Graphics/Render/CommandBuffers.h"
#include "Graphics/Scene/Scene.h"

/// <summary>
/// Handles rendering for the scene geometry.
/// </summary>
class SceneRender
{
public:
	SceneRender();
	SceneRender(const SceneRender&) = delete;
	SceneRender& operator=(const SceneRender&) = delete;
	~SceneRender() = default;

	/// <summary>
	/// Render the scene.
	/// </summary>
	/// <param name="scene">The scene to render.</param>
	/// <param name="render_buffers">Buffers for indirect drawing of models.
	/// </param>
	/// <param name="gBuffer">The buffer for geometry data.</param>
	/// <param name="command_buffers">The render command buffers.</param>
	void render(const Scene& scene, const RenderBuffers& render_buffers,
		const GBuffer& gBuffer, const CommandBuffers& command_buffers);

	/// <summary>
	/// Set up the uniforms for materials. This should be done when the
	/// materials that are loaded change, preferably infrequently.
	/// </summary>
	/// <param name="scene">The scene to pull materials from.</param>
	void setup_materials_uniform(const Scene& scene);
private:
	std::unique_ptr<ShaderProgram> shader_program;
	std::unique_ptr<UniformsMap> uniforms_map;

	/// <summary>
	/// A cached set of bindings from texture name to ID.
	/// </summary>
	std::map<std::string, int> texture_bindings;

	/// <summary>
	/// Set up uniforms for the shader.
	/// </summary>
	void create_uniforms();
};
