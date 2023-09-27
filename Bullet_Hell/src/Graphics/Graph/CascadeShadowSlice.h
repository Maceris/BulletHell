#pragma once

#include "Globals.h"

#include "glm.hpp"

#include "Scene.h"

/// <summary>
/// Used for cascaded shadow mapping, defines details for each slice like the 
/// view matrix and distance to this split.
/// </summary>
struct CascadeShadowSlice
{
	/// <summary>
	/// The number of sections to split the frustum into.
	/// </summary>
	static const unsigned int SHADOW_MAP_CASCADE_COUNT = 3;

	/// <summary>
	/// The combined projection and view matrix for lights.
	/// </summary>
	glm::mat4 projection_view_matrix;

	/// <summary>
	/// The distance to the split that this slice represents.
	/// </summary>
	float split_distance;
	
	/// <summary>
	/// Update the projection matrices for each slice based on the scene's
	/// directional light.
	/// </summary>
	/// <param name="shadows">The cascade shadows to update.</param>
	/// <param name="scene"></param>
	static void updateCascadeShadows(
		CascadeShadowSlice (& cascade_shadows)[SHADOW_MAP_CASCADE_COUNT],
		Scene& scene);
};