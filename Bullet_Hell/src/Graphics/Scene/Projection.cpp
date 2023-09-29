#include "Projection.h"

#include "gtc/matrix_transform.hpp"

const float Z_NEAR = 0.01f;
const float Z_FAR = 1000.0f;

Projection::Projection(const unsigned int width, const unsigned int height)
	: projection_matrix(glm::mat4(1.0f))
	, inverse_projection_matrix(glm::inverse(projection_matrix))
{
	update_matrices(width, height);
}

void Projection::update_matrices(const unsigned int width,
	const unsigned int height)
{
	projection_matrix = glm::perspectiveFov(FOV, (float) width, 
		(float) height, Z_NEAR, Z_FAR);
	inverse_projection_matrix = glm::inverse(projection_matrix);
}