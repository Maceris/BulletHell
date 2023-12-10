#pragma once

#include "Globals.h"

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "Event/Event.h"
#include "Graphics/Scene/Camera.h"
#include "Graphics/Scene/Fog.h"
#include "Graphics/Scene/Projection.h"
#include "Graphics/Scene/SceneCluster.h"
#include "Graphics/Scene/SkyBox.h"
#include "Graphics/Scene/Lights/SceneLights.h"
#include "Map/ChunkCoordinates.h"

struct Entity;
struct Model;
struct Tile;

/// <summary>
/// If we clean out models without any associated entities while pruning dead
/// entities. If this is true (1), when spawning things we should check that
/// the model actaully exists in the scene.
/// </summary>
#define PRUNE_MODELS_WITHOUT_ENTITIES 0

/// <summary>
/// A scene to be rendered, including entities and lighting.
/// </summary>
class Scene
{
#if DEBUG
	friend class DebugUI;
#endif
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
	/// All of the lights in the scene.
	/// </summary>
	SceneLights scene_lights;

	/// <summary>
	/// The backdrop.
	/// </summary>
	SkyBox sky_box;

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
	/// Go through and remove models that are pending removal.
	/// </summary>
	void prune_models();

	/// <summary>
	/// Update the projection matrix after the window is resized.
	/// </summary>
	/// <param name="width">The new window width, in pixels.</param>
	/// <param name="height">The new window height, in pixels.</param>
	void resize(const unsigned int width, const unsigned int height);

	/// <summary>
	/// Whether the scene has been modified since last time we rendered.
	/// 
	/// Since some things are exposed to modification externally, a value of 
	/// true means that the scene is definitely dirty, but false only means
	/// that it's probably not dirty.
	/// </summary>
	std::atomic_bool dirty = true;

	/// <summary>
	/// Whether the animated entities have been changed since we have rendered.
	/// </summary>
	std::atomic_bool animated_entities_dirty = true;

	/// <summary>
	/// Whether the animated models have been changed since we have rendered.
	/// </summary>
	std::atomic_bool animated_models_dirty = true;

	/// <summary>
	/// Whether the static entities have been changed since we have rendered.
	/// </summary>
	std::atomic_bool static_entities_dirty = true;

	/// <summary>
	/// Whether the static models have been changed since we have rendered.
	/// </summary>
	std::atomic_bool static_models_dirty = true;

	/// <summary>
	/// Tracks the rendering related things that we have loaded for each chunk.
	/// </summary>
	std::map<ChunkCoordinates, std::shared_ptr<SceneCluster>> chunk_contents;

	/// <summary>
	/// Fetch a list of all the models currently loaded in the scene. This
	/// includes all of the chunks, and every model should be unique.
	/// </summary>
	/// <returns>All of the scenes models.</returns>
	const std::vector<std::shared_ptr<Model>>& get_model_list() const;

	/// <summary>
	/// Fetch a list of all the static models currently loaded in the scene.
	/// This includes all of the chunks, and every model should be unique.
	/// </summary>
	/// <returns>All of the scenes models.</returns>
	const std::vector<std::shared_ptr<Model>>& get_static_model_list() const;

	/// <summary>
	/// Fetch a list of all the animated models currently loaded in the scene.
	/// This includes all of the chunks, and every model should be unique.
	/// </summary>
	/// <returns>All of the scenes models.</returns>
	const std::vector<std::shared_ptr<Model>>& get_animated_model_list() const;

	/// <summary>
	/// Recalculate the list of models when something has changed, since we
	/// cache the list to save redundant calculations several times a frame.
	/// </summary>
	void rebuild_model_lists();

private:

	std::vector<std::shared_ptr<Model>> cached_model_list;
	std::vector<std::shared_ptr<Model>> cached_static_model_list;
	std::vector<std::shared_ptr<Model>> cached_animated_model_list;

	/// <summary>
	/// A list of entities that we have added, but which we did not yet have
	/// models for.
	/// </summary>
	std::vector<std::shared_ptr<Entity>> entities_pending_models;

	/// <summary>
	/// A map of IDs to the corresponding model.
	/// </summary>
	std::map<const std::string, std::shared_ptr<Model>> model_map;

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