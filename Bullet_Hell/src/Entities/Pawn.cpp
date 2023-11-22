#include "Entities/Pawn.h"

Pawn::Pawn()
	: scene_entity{}
	, health{ 0 }
	, max_health{ 0 }
	, desired_movement{ 0, 0 }
	, desired_facing{ 0 }
{}

Pawn::Pawn(std::shared_ptr<Entity> entity, Health health)
	: scene_entity{ entity }
	, health{ health }
	, max_health{ health }
	, desired_movement{ 0, 0 }
	, desired_facing{ 0 }
{}
