#include "graphics/scene/projection.h"

#include "debugging/logger.h"
#include "glm/gtc/matrix_transform.hpp"

Projection::Projection(const unsigned int width, const unsigned int height)
	: projection_matrix{ 1.0f }
	, inverse_projection_matrix{ glm::inverse(projection_matrix) }
	, FOV{ glm::radians(60.0f) }
{
	update_matrices(width, height);
}

void Projection::update_matrices(const unsigned int width,
	const unsigned int height)
{
	if (height == 0)
	{
		LOG_WARNING("Updating projection matrix with a height of 0");
		return;
	}
	
	projection_matrix = glm::perspectiveLH_NO(FOV,
		static_cast<float>(width) / height, Z_NEAR, Z_FAR);
	projection_matrix[2][2] *= -1.0f;
	projection_matrix[2][3] *= -1.0f;

	inverse_projection_matrix = glm::inverse(projection_matrix);
}