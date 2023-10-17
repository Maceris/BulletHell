#pragma once

#include "Globals.h"

#include <memory>

#include "GBuffer.h"
#include "QuadMesh.h"
#include "Scene.h"
#include "ShaderProgram.h"
#include "ShadowRender.h"
#include "UniformsMap.h"

#include "glad.h"
#include "glm.hpp"

/// <summary>
/// Handles light rendering.
/// </summary>
class LightRender
{
public:
	LightRender();
	LightRender(const LightRender&) = delete;
	LightRender& operator=(const LightRender&) = delete;
	~LightRender() = default;

	/// <summary>
	/// Render all the lighting for a scene.
	/// </summary>
	/// <param name="scene">The scene we are rendering.</param>
	/// <param name="shadow_render">The shadow renderer, which we 
	/// need for cascade shadow information.</param>
	/// <param name="gBuffer">The geometry buffer.</param>
	void render(const Scene& scene, const ShadowRender& shadow_render,
		const GBuffer& gBuffer);
private:
	/// <summary>
	/// The GUI Shader program.
	/// </summary>
	std::unique_ptr<ShaderProgram> shader_program;

	/// <summary>
	/// The mesh we render to.
	/// </summary>
	std::unique_ptr<QuadMesh> quad_mesh;

	/// <summary>
	/// Uniforms for the shader.
	/// </summary>
	std::unique_ptr<UniformsMap> uniforms_map;

	void create_uniforms();
	void update_lights(const Scene& scene);
	void update_point_light(const PointLight* point_light, 
		const std::string& prefix, const glm::mat4& view_matrix);
	void update_spot_light(const SpotLight* spot_light,
		const std::string& prefix, const glm::mat4& view_matrix);
};
