#include "entities/pawn.h"

#include "graphics/scene/entity.h"

Pawn::Pawn()
	: desired_facing{ 0.0f, 0.0f }
	, desired_movement{ 0.0f, 0.0f }
	, health{ 0 }
	, max_health{ 0 }
	, scene_entity{}
	, state{ AIState::IDLE }
{}

Pawn::Pawn(std::shared_ptr<Entity> entity, Health health)
	: desired_facing{ 0.0f, 0.0f }
	, desired_movement{ 0.0f, 0.0f }
	, health{ health }
	, max_health{ health }
	, scene_entity{ entity }
	, state{ AIState::IDLE }
{}

Pawn::~Pawn()
{
	scene_entity->dead = true;
}