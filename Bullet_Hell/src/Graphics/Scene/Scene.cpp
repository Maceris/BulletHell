#include "Graphics/Scene/Scene.h"

#include "Delegate.h"

#include "Debugging/Logger.h"
#include "Event/Map/ChunkLoaded.h"
#include "Event/Map/ChunkUnloaded.h"
#include "Main/GameLogic.h"
#include "Graphics/Graph/ModelResource.h"
#include "Graphics/Scene/Entity.h"

Scene::Scene(const unsigned int width, const unsigned int height)
	: camera{}
	, fog{}
	, projection{width, height}
	, model_map{}
	, scene_lights{}
	, sky_box{}
	, player{}
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
	}
	else
	{
		entities_pending_models.push_back(entity);
	}
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
}

void Scene::resize(const unsigned int width, const unsigned int height)
{
	projection.update_matrices(width, height);
}

void Scene::handle_chunk_loading(EventPointer event)
{
	LOG_ASSERT(event && "Our event is null");
	std::shared_ptr<ChunkLoaded> loaded_event = 
		std::static_pointer_cast<ChunkLoaded>(event);

	const Tile (*tiles)[CHUNK_WIDTH][CHUNK_WIDTH] = &(loaded_event->chunk->tiles);
	const ChunkCoordinates* chunk_location = &(loaded_event->chunk->location);

	SceneCluster cluster;

	for (int x = 0; x < CHUNK_WIDTH; ++x)
	{
		for (int z = 0; z < CHUNK_WIDTH; ++z)
		{
			const Tile* tile = tiles[x][z];
			const int world_x = chunk_location->x * CHUNK_WIDTH + x;
			const int world_z = chunk_location->z * CHUNK_WIDTH + z;
			load_tile(world_x, world_z, *tile, cluster);
		}
	}

	chunk_contents.insert(std::make_pair(*chunk_location, cluster));
}

void Scene::handle_chunk_unloading(EventPointer event)
{
	//TODO(ches) handle unloading models for all the tiles in the chunk
	LOG_INFO("We are doing something with a loaded chunk");
}

void Scene::load_tile(const int& x, const int& z, const Tile& tile,
	SceneCluster& cluster)
{
	const char* model_name;

	switch (tile.id)
	{
	case TILE_GROUND:
		model_name = "models/map/tile_001.model";
		break;
	case TILE_VOID:
	default:
		 model_name = "";
		 break;
	}

	if (model_name && model_name != "")
	{
		//TODO(ches) Fix logic 
		auto tile_model = load_model(model_name);
		auto tile_entity = std::make_shared<Entity>(tile_model->id);
		
		tile_entity->set_position(x, 0, z);
		tile_entity->update_model_matrix();

		cluster.entities[tile_model].push_back(tile_entity);
	}
}
