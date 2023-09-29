#pragma once

#include "Globals.h"

#include "glad.h"

#include "Scene.h"
#include "ShaderProgram.h"
#include "RenderBuffers.h"
#include "UniformsMap.h"

#include <memory>

/// <summary>
/// Handles compute shaders for animated models.
/// </summary>
class AnimationRender
{
private:
	std::unique_ptr<ShaderProgram> shader_program;
	std::unique_ptr<UniformsMap> uniforms_map;

public:
	AnimationRender();
	AnimationRender(const AnimationRender&) = delete;
	AnimationRender& operator=(const AnimationRender&) = delete;
	~AnimationRender() = default;

	void render(const Scene& scene, const RenderBuffers& render_buffer);
};
