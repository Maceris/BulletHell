#pragma once

#include "Globals.h"

#include <memory>

#include "Graphics/Graph/GBuffer.h"
#include "Graphics/Graph/QuadMesh.h"
#include "Graphics/Graph/ShaderProgram.h"
#include "Graphics/Graph/UniformsMap.h"
#include "Graphics/Render/ShadowRender.h"
#include "Graphics/Scene/Scene.h"

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
	void render(const Scene& scene, ShadowRender& shadow_render,
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
	void setup_point_light_buffer(const Scene& scene);
	void setup_spot_light_buffer(const Scene& scene);
	void initialize_SSBOs();

	GLuint point_light_buffer;
	GLuint spot_light_buffer;
};
