#include "Graphics/Scene/Scene.h"

#include <unordered_set>

#include "Delegate.h"

#include "Debugging/Logger.h"
#include "Event/EventManager.h"
#include "Event/Map/ChunkLoaded.h"
#include "Event/Map/ChunkUnloaded.h"
#include "Main/GameLogic.h"
#include "Map/Chunk.h"
#include "Map/Tile.h"
#include "Graphics/Graph/ModelResource.h"
#include "Graphics/Scene/Entity.h"
#include "Graphics/Graph/Model.h"

Scene::Scene(const unsigned int width, const unsigned int height)
	: camera{}
	, fog{}
	, projection{width, height}
	, model_map{}
	, scene_lights{}
	, sky_box{}
{
	g_event_manager->register_handler(
		EventHandler::create<Scene, &Scene::handle_chunk_loading>(this),
		ChunkLoaded::event_type
	);
	g_event_manager->register_handler(
		EventHandler::create<Scene, &Scene::handle_chunk_unloading>(this),
		ChunkUnloaded::event_type
	);
}

void Scene::add_entity(std::shared_ptr<Entity> entity)
{
	// We might want to load models and/or entities from multiple threads
	std::scoped_lock<std::mutex> lock(pending_models_mutex);

	const std::string& model_ID = entity->model_ID;
	auto result = model_map.find(model_ID);

	if (result != model_map.end())
	{
		auto& vec = result->second->entity_list;
		vec.push_back(entity);
		if (result->second->is_animated())
		{
			animated_models_dirty = true;
		}
		else
		{
			static_models_dirty = true;
		}
	}
	else
	{
		entities_pending_models.push_back(entity);
	}
	dirty = true;
	
}

void Scene::add_model(std::shared_ptr<Model> model)
{
	model_map.emplace(std::make_pair(model->id, model));
	for (auto it = entities_pending_models.begin(); 
		it < entities_pending_models.end();)
	{
		std::string current_entities_model = it->get()->model_ID;
		if (model->id == current_entities_model)
		{
			model->entity_list.push_back(*it);
			it = entities_pending_models.erase(it);
		}
		else
		{
			++it;
		}
	}
	dirty = true;

	if (model->is_animated())
	{
		animated_models_dirty = true;
	}
	else
	{
		static_models_dirty = true;
	}
}

void Scene::resize(const unsigned int width, const unsigned int height)
{
	projection.update_matrices(width, height);
}

const std::vector<std::shared_ptr<Model>>& Scene::get_model_list() const
{
	return cached_model_list;
}

const std::vector<std::shared_ptr<Model>>& Scene::get_static_model_list() const
{
	return cached_static_model_list;
}

const std::vector<std::shared_ptr<Model>>& Scene::get_animated_model_list() const
{
	return cached_animated_model_list;
}

void Scene::rebuild_model_lists()
{
	cached_model_list.clear();
	cached_static_model_list.clear();
	cached_animated_model_list.clear();

	std::unordered_set<std::shared_ptr<Model>> models;

	for (auto& entry : model_map)
	{
		models.insert(entry.second);
	}

	for (auto& cluster_pair : chunk_contents)
	{
		auto& entity_map = cluster_pair.second->entities;
		for (auto& pair : entity_map)
		{
			models.insert(pair.first);
		}
	}

	for (auto& model : models)
	{
		cached_model_list.push_back(model);
		if (model->is_animated())
		{
			cached_animated_model_list.push_back(model);
		}
		else
		{
			cached_static_model_list.push_back(model);
		}
	}
}

void Scene::handle_chunk_loading(EventPointer event)
{
	LOG_ASSERT(event && "Our event is null");
	std::shared_ptr<ChunkLoaded> loaded_event = 
		std::static_pointer_cast<ChunkLoaded>(event);

	const Tile (*tiles)[CHUNK_WIDTH][CHUNK_WIDTH] = &(loaded_event->chunk->tiles);
	const ChunkCoordinates* chunk_location = &(loaded_event->chunk->location);

	std::shared_ptr<SceneCluster> cluster = std::make_shared<SceneCluster>();

	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int z = 0; z < CHUNK_WIDTH; ++z)
		{
			const Tile tile = (*tiles)[x][z];
			const int world_x = chunk_location->x * CHUNK_WIDTH + x;
			const int world_z = chunk_location->z * CHUNK_WIDTH + z;
			load_tile(world_x, world_z, tile, *cluster);
		}
	}

	chunk_contents.insert(std::make_pair(*chunk_location, cluster));
	LOG_INFO("Loading a chunk, now we have "
		+ std::to_string(chunk_contents.size()) + " chunks right now.");

	dirty = true;
	static_models_dirty = true;
}

void Scene::handle_chunk_unloading(EventPointer event)
{
	LOG_ASSERT(event && "Our event is null");
	std::shared_ptr<ChunkUnloaded> unloaded_event =
		std::static_pointer_cast<ChunkUnloaded>(event);

	const auto& cluster = chunk_contents.find(unloaded_event->coordinates);

	LOG_ASSERT(cluster != chunk_contents.end()
		&& "Unloading a chunk that does not appear to be loaded");

	for (auto& entity_mapping : cluster->second->entities)
	{
		EntityList to_keep;
		auto& entity_list = entity_mapping.first->entity_list;
		for (auto& existing : entity_list)
		{
			bool found = false;
			for (auto& to_remove : entity_mapping.second)
			{
				if (to_remove->entity_ID == existing->entity_ID)
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				to_keep.push_back(existing);
			}
		}

		entity_list.clear();
		for (auto& entity : to_keep)
		{
			entity_list.push_back(entity);
		}
	}

	chunk_contents.erase(unloaded_event->coordinates);

	dirty = true;
	static_models_dirty = true;
}

void Scene::load_tile(const int& x, const int& z, const Tile& tile,
	SceneCluster& cluster)
{
	std::string model_name = "";

	switch (tile.id)
	{
	case TILE_GROUND:
		model_name = "models/map/tile_001.model";
		break;
	case TILE_VOID:
		//NOTE(ches) Nothing required here.
		break;
	default:
		LOG_ERROR("Unsupported tile type");
		 break;
	}

	if (model_name != "")
	{
		auto tile_model = load_model(model_name);
		auto tile_entity = std::make_shared<Entity>(tile_model->id);
		tile_model->entity_list.push_back(tile_entity);
		tile_entity->scale = TILE_SCALE;
		tile_entity->set_position(x * TILE_SCALE * 2, 0.0f, 
			z * TILE_SCALE * 2);
		tile_entity->update_model_matrix();

		cluster.entities[tile_model].push_back(tile_entity);
	}
}
