#include "Entity.h"

Entity::Entity(const std::string entity_ID, const std::string model_ID)
	: entity_ID(entity_ID)
	, model_ID(model_ID)
	, model_matrix(glm::mat4(1))
	, position(glm::vec3(0))
	, rotation(glm::quat())
	, scale(1)
{}

void Entity::add_rotation(const float x, const float y, const float z,
	const float angle)
{
	glm::quat delta = glm::angleAxis(glm::radians(angle), 
		glm::normalize(glm::vec3(x, y, z)));
	rotation = delta * rotation;
}

void Entity::set_position(const float x, const float y, const float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void Entity::set_rotation(const float x, const float y, const float z,
	const float angle)
{
	rotation = glm::angleAxis(glm::radians(angle),
		glm::normalize(glm::vec3(x, y, z)));
}

void Entity::update_model_matrix()
{
	model_matrix = glm::mat4(1);
	model_matrix = glm::scale(model_matrix, glm::vec3(scale, scale, scale));
	glm::mat4 translation = glm::translate(model_matrix, position);
	glm::mat4 rot = glm::toMat4(rotation);
	model_matrix = rot * model_matrix;
}