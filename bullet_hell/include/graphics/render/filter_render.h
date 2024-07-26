#pragma once

#include <string>
#include <memory>

#include "graphics/glad_types.h"

struct QuadMesh;
class Scene;
class ShaderProgram;
struct UniformsMap;

/// <summary>
/// Handles post-processing filters
/// </summary>
class FilterRender
{
private:
	/// <summary>
	/// The current shader, we should always have at least the default.
	/// </summary>
	std::shared_ptr<ShaderProgram> shader;

	/// <summary>
	/// Uniforms for the current shader.
	/// </summary>
	std::unique_ptr<UniformsMap> uniforms_map;

	/// <summary>
	/// The mesh we render to.
	/// </summary>
	std::unique_ptr<QuadMesh> quad_mesh;

public:
	FilterRender();
	FilterRender(const FilterRender&) = delete;
	FilterRender& operator=(const FilterRender&) = delete;
	~FilterRender() = default;

	void render(const Scene& scene, const GLuint screen_texture);
	/// <summary>
	/// Set the filter to use by name. This expects the path to the shader
	/// asset, including the name of the shader but not including the
	/// extension. The extensions ".frag" and ".vert" are assumed.
	/// 
	/// As an example, "shaders/filters/default" would be the name provided
	/// for a filter consisting of "shaders/filters/default.frag" and 
	/// "shaders/filters/default.vert".
	/// </summary>
	/// <param name="name">The name of the shader to load and set as the
	/// shader program.</param>
	/// <returns>Whether we found and successfully set the filter.</returns>
	bool set_filter(const std::string name);
};
