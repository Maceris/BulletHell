#pragma once

#include <vector>

#include "graphics/scene/lights/ambient_light.h"
#include "graphics/scene/lights/directional_light.h"
#include "graphics/scene/lights/point_light.h"
#include "graphics/scene/lights/spot_light.h"

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

	SceneLights(const SceneLights&) = delete;
	SceneLights& operator=(const SceneLights&) = delete;
	~SceneLights() = default;
};