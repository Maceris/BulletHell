#include "Entity.h"

std::atomic<uint64_t> Entity::next_ID{ 0 };

Entity::Entity(const std::string model_ID)
	: entity_ID{ next_ID++ }
	, model_ID{ model_ID }
	, model_matrix{ 1.0f }
	, position{ 0.0f }
	, rotation{}
	, scale{ 1 }
	, animation_data{ std::shared_ptr<Animation>() }
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
	model_matrix = glm::mat4(1.0f);
	glm::mat4 temp_rotation = glm::toMat4(rotation);
	glm::mat4 translation{ 1 };
	translation = glm::translate(model_matrix, position);
	glm::mat4 temp_scale{ 1 };
	temp_scale = glm::scale(temp_scale, glm::vec3(scale, scale, scale));
	model_matrix = translation * temp_rotation * temp_scale * model_matrix;
}