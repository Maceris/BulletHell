#include "Graphics/Scene/Scene.h"

#include "Delegate.h"

#include "Debugging/Logger.h"
#include "Main/GameLogic.h"
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
	//TODO(ches) handle loading models for all the tiles in the chunk
	LOG_INFO("We are doing something with a loaded chunk");
}