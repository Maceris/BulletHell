#pragma once

#include "globals.h"

#include "glm/vec2.hpp"

struct Pawn;

/// <summary>
/// Does calculations relating to AI.
/// </summary>
class Brain
{
public:
	/// <summary>
	/// Update the desired actions based on the current state.
	/// </summary>
	/// <param name="enemy">The enemy pawn to update.</param>
	static void update(Pawn& enemy, const Pawn& player);
};