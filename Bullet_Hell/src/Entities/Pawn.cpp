#include "Entities/Pawn.h"

Pawn::Pawn()
	: desired_facing{ 0 }
	, desired_movement{ 0, 0 }
	, health{ 0 }
	, max_health{ 0 }
	, scene_entity{}
	, state{ AIState::IDLE }
{}

Pawn::Pawn(std::shared_ptr<Entity> entity, Health health)
	: desired_facing{ 0 }
	, desired_movement{ 0, 0 }
	, health{ health }
	, max_health{ health }
	, scene_entity{ entity }
	, state{ AIState::IDLE }
{}
