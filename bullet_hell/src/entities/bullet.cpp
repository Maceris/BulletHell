#include "Entities/Bullet.h"

Bullet::Bullet(const Health damage, const glm::vec2& direction, 
	const float speed, std::shared_ptr<Entity> scene_entity)
	: damage{ damage }
	, direction{ direction }
	, speed{ speed }
	, scene_entity{ scene_entity }
{}

Bullet::~Bullet()
{
	scene_entity->dead = true;
}