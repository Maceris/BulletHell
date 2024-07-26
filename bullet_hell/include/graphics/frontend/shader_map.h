#pragma once

#include <map>
#include <memory>

#include "graphics/frontend/render_stage.h"
#include "graphics/frontend/shader.h"

struct ShaderMap
{
	ShaderMap();
	ShaderMap(const ShaderMap&) = delete;
	ShaderMap& operator=(const ShaderMap&) = delete;
	~ShaderMap();

	/// <summary>
	/// Add a new shader to the map. If one is already present for the
	/// specified stage, the existing entry will be deleted.
	/// </summary>
	/// <param name="type">The stage to add a shser for.</param>
	/// <param name="shader">The new shader.</param>
	void add_shader(RenderStage::Type type, Shader* shader);

	/// <summary>
	/// Deletes all shaders in the map.
	/// </summary>
	void clear_all();

	/// <summary>
	/// Remove a specific shader, without replacing it.
	/// </summary>
	/// <param name="type">The render stage to delete a shader for.</param>
	void remove_shader(RenderStage::Type type);

	/// <summary>
	/// Fetch the shader for a specific stage.
	/// </summary>
	/// <param name="type">The stage type to fetch a shader program for.</param>
	/// <returns>A pointer to the shader object, or nullptr if not found.
	/// </returns>
	Shader* get_shader(RenderStage::Type type);

private:
	std::map<RenderStage::Type, Shader*> shaders;
};