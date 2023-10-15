#include "Model.h"

Model::Model(const std::string id, std::vector<MeshData> mesh_data_list,
	std::vector<std::string> animation_list)
	: id(id)
	, mesh_data_list(std::move(mesh_data_list))
	, animation_list(std::move(animation_list))
	, entity_list(std::vector<std::shared_ptr<Entity>>())
	, mesh_draw_data_list(std::vector<MeshDrawData>())
{}

bool Model::is_animated()
{
	return !animation_list.empty();
}