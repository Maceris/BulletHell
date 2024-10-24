#pragma once

#include <memory>

#include "graphics/frontend/uniforms_map.h"
#include "graphics/graph/shader_program.h"

class Scene;

// Handles rendering for the skybox
class SkyBoxRender
{
public:
	SkyBoxRender();
	SkyBoxRender(const SkyBoxRender&) = delete;
	SkyBoxRender& operator=(const SkyBoxRender&) = delete;
	~SkyBoxRender() = default;

	/// <summary>
	/// Render the skybox.
	/// </summary>
	/// <param name="scene">The scene we are rendering.</param>
	void render(const Scene& scene);
private:
	std::unique_ptr<ShaderProgram> shader_program;
	std::unique_ptr<UniformsMap> uniforms_map;

	/// <summary>
	/// Set up uniforms for this stage.
	/// </summary>
	void create_uniforms();
};
