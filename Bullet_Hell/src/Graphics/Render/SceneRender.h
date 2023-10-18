#pragma once

#include "Globals.h"

#include <memory>

#include "CommandBuffers.h"
#include "GBuffer.h"
#include "RenderBuffers.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "UniformsMap.h"

#include "glad.h"

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

	void render(const Scene& scene, const RenderBuffers& render_buffers,
		const GBuffer& gBuffer, const CommandBuffers& command_buffers);

	/// <summary>
	/// Set up the uniforms for materials. This should be done when the
	/// materials that are loaded change, preferably infrequently.
	/// </summary>
	/// <param name="scene">The scene to pull materials from.</param>
	void setup_materials_uniform(const Scene& scene);
private:
	/// <summary>
	/// The GUI Shader program.
	/// </summary>
	std::unique_ptr<ShaderProgram> shader_program;

	/// <summary>
	/// Uniforms for the shader.
	/// </summary>
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
