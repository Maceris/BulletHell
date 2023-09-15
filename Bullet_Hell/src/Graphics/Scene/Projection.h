#pragma once

#include "Globals.h"

#include "mat4x4.hpp"
#include "gtc/matrix_transform.hpp"

class Projection
{
public:
	
	glm::mat4 inverse_projection_matrix;
	glm::mat4 projection_matrix;
	
	Projection(const unsigned int width, const unsigned int height);
	void update_matrices(const unsigned int width, const unsigned int height);

private:
	const float Z_FAR = 1000.0f;
	const float Z_NEAR = 0.01f;
	const double DEGREES_TO_RADIANS = 0.017453292519943295;
	const float FOV = 60 * DEGREES_TO_RADIANS;
};