#pragma once

#include <unordered_map>
#include <vector>

#include "graphics/graph/model.h"
#include "graphics/scene/entity.h"
#include "graphics/scene/lights/point_light.h"
#include "graphics/scene/lights/spot_light.h"

/// <summary>
/// A group of things to render in the scene.
/// </summary>
struct SceneCluster
{
	/// <summary>
	/// The list of point lights in the scene.
	/// </summary>
	std::vector<PointLight> point_lights;

	/// <summary>
	/// The list of spot lights in a scene.
	/// </summary>
	std::vector<SpotLight> spot_lights;

	/// <summary>
	/// A map from model names to the list of the entities of that model.
	/// </summary>
	std::unordered_map<std::string, EntityList> entities;
};