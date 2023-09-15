#include "Projection.h"

Projection::Projection(const unsigned int width, const unsigned int height)
	: projection_matrix(glm::mat4())
	, inverse_projection_matrix(glm::mat4())
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