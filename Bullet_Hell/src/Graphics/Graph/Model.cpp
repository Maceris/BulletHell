#include "Model.h"

Model::Model(const std::string id)
	: id{ id }
	, mesh_data_list{}
	, animation_list{}
	, entity_list{}
	, mesh_draw_data_list{}
{}

bool Model::is_animated()
{
	return !animation_list.empty();
}