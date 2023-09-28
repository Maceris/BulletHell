#pragma once

#include "Globals.h"

#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include <vector>

/// <summary>
/// Lighting information for a scene.
/// </summary>
struct SceneLights
{
	/// <summary>
	/// Ambient lighting details.
	/// </summary>
	AmbientLight ambient_light;

	/// <summary>
	/// Directional (like sun, sky) lighting details.
	/// </summary>
	DirectionalLight directional_light;

	/// <summary>
	/// The list of point lights in the scene.
	/// </summary>
	std::vector<PointLight> point_lights;

	/// <summary>
	/// The list of spot lights in a scene.
	/// </summary>
	std::vector<SpotLight> spot_lights;

	/// <summary>
	/// Create a new scene light setup without any point or spot lights
	/// configured.
	/// </summary>
	SceneLights();

	/// <summary>
	/// Do not allow copy.
	/// </summary>
	SceneLights(const SceneLights&) = delete;

	/// <summary>
	/// Do not allow copy.
	/// </summary>
	SceneLights& operator=(const SceneLights&) = delete;

	/// <summary>
	/// Clean up lights.
	/// </summary>
	~SceneLights() = default;
};