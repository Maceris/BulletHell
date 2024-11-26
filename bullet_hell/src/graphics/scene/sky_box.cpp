#include "graphics/scene/sky_box.h"

#include "debugging/logger.h"
#include "graphics/graph/mesh_data.h"
#include "graphics/graph/model.h"
#include "graphics/graph/model_resource.h"
#include "main/game_logic.h"
#include "resource_cache/resource_cache.h"

SkyBox::SkyBox()
{
	const std::string model_name = "models/skybox/skybox.model";
	model = load_model(model_name);
	entity = std::make_shared<Entity>(model_name);

	LOG_ASSERT(model->mesh_data_list.size() == 1
		&& "We are assuming that skybox models only have one mesh");
}
