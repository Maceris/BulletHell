#pragma once

#include <array>
#include <vector>

#include "glm/glm.hpp"

#include "graphics/render_constants.h"

class Scene;

/// <summary>
/// Used for cascaded shadow mapping, defines details for each slice like the 
/// view matrix and distance to this split.
/// </summary>
struct CascadeShadowSlice
{
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
	/// <param name="scene">The scene we are updating shadows for.</param>
	static void updateCascadeShadows(
		std::array<CascadeShadowSlice, SHADOW_MAP_CASCADE_COUNT>& shadows,
		const Scene& scene);

	CascadeShadowSlice();
	CascadeShadowSlice(const CascadeShadowSlice&) = default;
	CascadeShadowSlice& operator=(const CascadeShadowSlice&) = default;

	~CascadeShadowSlice() = default;

private:
	/// <summary>
	/// We calculate the splits once, and just reuse them.
	/// </summary>
	static float* cached_splits;
};

/// <summary>
/// An appropriately sized (see SHADOW_MAP_CASCADE_COUNT) array of
/// Cascade Shadow Slices.
/// </summary>
using CascadeShadows = std::array<CascadeShadowSlice, SHADOW_MAP_CASCADE_COUNT>;
