#include "Scene.h"

#include "Entity.h"

Scene::Scene(const unsigned int width, const unsigned int height)
	: camera(Camera())
	, fog(Fog())
	, projection(Projection(width, height))
	, model_map{}
	, scene_lights(SceneLights())
	, sky_box(SkyBox())
{}

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