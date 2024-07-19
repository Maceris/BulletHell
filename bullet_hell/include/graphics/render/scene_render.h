#pragma once

#include "globals.h"

#include <memory>
#include <string>
#include <vector>

#include "graphics/graph/shader_program.h"
#include "graphics/graph/uniforms_map.h"

struct CommandBuffers;
struct GBuffer;
class RenderBuffers;
class Scene;

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

	
private:
	std::unique_ptr<ShaderProgram> shader_program;
	std::unique_ptr<UniformsMap> uniforms_map;

	/// <summary>
	/// Set up uniforms for the shader.
	/// </summary>
	void create_uniforms();

	/// <summary>
	/// Set up the uniforms for models in the scene.
	/// </summary>
	/// <param name="scene">The model we are going to render.</param>
	void setup_materials_uniform(const Scene& scene, const bool animated);
};
