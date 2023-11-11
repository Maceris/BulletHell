#pragma once

#include "Globals.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "Event/Map/ChunkLoaded.h"
#include "Event/Map/ChunkUnloaded.h"
#include "Graphics/Graph/Model.h"
#include "Graphics/Scene/Camera.h"
#include "Graphics/Scene/Entity.h"
#include "Graphics/Scene/Fog.h"
#include "Graphics/Scene/Projection.h"
#include "Graphics/Scene/SceneCluster.h"
#include "Graphics/Scene/SkyBox.h"
#include "Graphics/Scene/Lights/SceneLights.h"

/// <summary>
/// A scene to be rendered, including entities and lighting.
/// </summary>
class Scene
{
public:
	/// <summary>
	/// Information about where the camera is and where it is looking.
	/// </summary>
	Camera camera;

	/// <summary>
	/// The details used to render fog.
	/// </summary>
	Fog fog;

	/// <summary>
	/// The projection matrix, taking into account screen size.
	/// </summary>
	Projection projection;

	/// <summary>
	/// A map of IDs to the corresponding model.
	/// </summary>
	std::map<const std::string, std::shared_ptr<Model>> model_map;

	/// <summary>
	/// All of the lights in the scene.
	/// </summary>
	SceneLights scene_lights;

	/// <summary>
	/// The backdrop.
	/// </summary>
	SkyBox sky_box;

	/// <summary>
	/// A reference to the entity representing the player character.
	/// </summary>
	std::shared_ptr<Entity> player;

	/// <summary>
	/// Set up a new scene.
	/// </summary>
	/// <param name="width">The window width, in pixels.</param>
	/// <param name="height">The window height, in pixels.</param>
	Scene(const unsigned int width, const unsigned int height);

	Scene(const Scene&) = delete;
	Scene& operator=(const Scene&) = delete;
	~Scene() = default;

	/// <summary>
	/// Registers the entity with it's associated model, if the model has
	/// already been loaded. Otherwise we will register with the model later,
	/// when it gets added to the scene.
	/// 
	/// Care should be taken to make sure all models get loaded before we
	/// try to draw anything in the scene.
	/// </summary>
	/// <param name="entity">The entity we want to add.</param>
	void add_entity(std::shared_ptr<Entity> entity);

	/// <summary>
	/// Add a model to the model map. If there are any entities that use this
	/// model, we add them to this models list of entities.
	/// </summary>
	/// <param name="model">The model to add.</param>
	void add_model(std::shared_ptr<Model> model);

	/// <summary>
	/// Update the projection matrix after the window is resized.
	/// </summary>
	/// <param name="width">The new window width, in pixels.</param>
	/// <param name="height">The new window height, in pixels.</param>
	void resize(const unsigned int width, const unsigned int height);

private:
	/// <summary>
	/// A list of entities that we have added, but which we did not yet have
	/// models for.
	/// </summary>
	std::vector<std::shared_ptr<Entity>> entities_pending_models;

	/// <summary>
	/// Tracks the rendering related things that we have loaded for each chunk.
	/// </summary>
	std::map<ChunkCoordinates, SceneCluster> chunk_contents;

	/// <summary>
	/// Used to synchronize access to the list of entities that are awaiting
	/// models.
	/// </summary>
	std::mutex pending_models_mutex;

	void handle_chunk_loading(EventPointer event);
	void handle_chunk_unloading(EventPointer event);

	/// <summary>
	/// Given a tile and coordinates, load the appropriate model.
	/// </summary>
	/// <param name="x">The global x coordinate of the tile.</param>
	/// <param name="z">The global x coordinate of the tile.</param>
	/// <param name="tile">The tile we are loading.</param>
	/// <param name="cluster">The cluster we are loading into.</param>
	void load_tile(const int& x, const int& z, const Tile& tile,
		SceneCluster& cluster);
};