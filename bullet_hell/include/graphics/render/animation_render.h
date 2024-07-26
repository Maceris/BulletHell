#pragma once

#include "globals.h"

#include <memory>

#include "graphics/backend/opengl/quad_mesh.h"
#include "graphics/frontend/uniforms_map.h"
#include "graphics/graph/shader_program.h"

class RenderBuffers;
class Scene;

/// <summary>
/// Handles compute shaders for animated models.
/// </summary>
class AnimationRender
{
private:
	std::unique_ptr<ShaderProgram> shader_program;
	std::unique_ptr<UniformsMap> uniforms_map;
	std::unique_ptr<QuadMesh> quad_mesh;

public:
	AnimationRender();
	AnimationRender(const AnimationRender&) = delete;
	AnimationRender& operator=(const AnimationRender&) = delete;
	~AnimationRender() = default;

	/// <summary>
	/// Send over information for the compute shaders for animations.
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="render_buffer"></param>
	void render(const Scene& scene, const RenderBuffers& render_buffer);
};
