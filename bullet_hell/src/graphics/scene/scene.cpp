#include "graphics/scene/scene.h"

#include <unordered_set>

#include "Delegate.h"

#include "debugging/logger.h"
#include "event/event_manager.h"
#include "event/map/chunk_loaded.h"
#include "event/map/chunk_unloaded.h"
#include "main/game_logic.h"
#include "map/chunk.h"
#include "map/tile.h"
#include "graphics/graph/model_resource.h"
#include "graphics/scene/entity.h"
#include "graphics/graph/model.h"

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

		if (vec.empty())
		{
			if (result->second->is_animated())
			{
				animated_models_dirty = true;
			}
			else
			{
				static_models_dirty = true;
			}
		}
		vec.push_back(entity);
		if (result->second->is_animated())
		{
			animated_entities_dirty = true;
		}
		else
		{
			static_entities_dirty = true;
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
	bool found_pending_model = false;
	for (auto it = entities_pending_models.begin(); 
		it < entities_pending_models.end();)
	{
		std::string current_entities_model = it->get()->model_ID;
		if (model->id == current_entities_model)
		{
			model->entity_list.push_back(*it);
			it = entities_pending_models.erase(it);
			found_pending_model = true;
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
		if (found_pending_model)
		{
			animated_entities_dirty = true;
		}
	}
	else
	{
		static_models_dirty = true;
		if (found_pending_model)
		{
			static_entities_dirty = true;
		}
	}
}

void Scene::prune_models()
{
	for (auto& model_mapping : model_map)
	{
		bool removed_any = false;

		EntityList to_keep;
		auto& entity_list = model_mapping.second->entity_list;
		for (auto& entity : entity_list)
		{
			if (!entity->dead)
			{
				to_keep.push_back(entity);
			}
			else
			{
				removed_any = true;
			}
		}
		if (removed_any)
		{
			entity_list.clear();
			for (auto& entity : to_keep)
			{
				entity_list.push_back(entity);
			}

			dirty = true;
			if (model_mapping.second->is_animated())
			{
				animated_entities_dirty = true;
			}
			else
			{
				static_entities_dirty = true;
			}
		}
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

	for (auto& model_mapping : model_map)
	{
		auto& model = model_mapping.second;
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

void Scene::reset()
{
	dirty = true;
	animated_entities_dirty = true;
	animated_models_dirty = true;
	static_entities_dirty = true;
	static_models_dirty = true;
	rebuild_model_lists();
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
		auto& entity_list = entity_mapping.second;
		for (auto& existing : entity_list)
		{
			existing->dead = true;
		}
		entity_list.clear();
	}

	chunk_contents.erase(unloaded_event->coordinates);

	dirty = true;
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
	case TILE_GROUND_DIRTY:
		model_name = "models/map/tile_002.model";
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
		std::shared_ptr<Model> tile_model;
		if (!model_map.contains(model_name))
		{
			tile_model = load_model(model_name);
			add_model(tile_model);
		}
		else
		{
			tile_model = model_map.find(model_name)->second;
		}
		auto tile_entity = std::make_shared<Entity>(tile_model->id);
		add_entity(tile_entity);
		tile_entity->scale = TILE_SCALE;
		tile_entity->set_position(x * TILE_SCALE * 2, 0.0f, 
			z * TILE_SCALE * 2);
		tile_entity->update_model_matrix();

		cluster.entities[model_name].push_back(tile_entity);
	}
}
