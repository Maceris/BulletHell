#pragma once

#include "Globals.h"

#include <unordered_map>
#include <vector>

#include "Graphics/Graph/Model.h"
#include "Graphics/Scene/Entity.h"
#include "Graphics/Scene/Lights/PointLight.h"
#include "Graphics/Scene/Lights/SpotLight.h"

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
	/// A map from models to the list of the entities of that model.
	/// </summary>
	std::unordered_map<std::shared_ptr<Model>, EntityList> entities;
};