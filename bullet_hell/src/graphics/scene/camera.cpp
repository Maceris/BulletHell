#include "graphics/scene/camera.h"

#include <numbers>

#include "utilities/math_util.h"

/// <summary>
/// A constant value used to clamp rotation, precomputed for convenience.
/// </summary>
constexpr float TWO_PI = (float)(2 * std::numbers::pi);

Camera::Camera()
	: position{ 0.0f }
	, rotation{ 0.0f }
	, view_matrix{ 1.0f }
	, inverse_view_matrix{ glm::inverse(view_matrix) }
{}

Camera::Camera(const Camera&) = default;
Camera& Camera::operator=(const Camera&) = default;
Camera::~Camera() = default;

void Camera::add_position(const glm::vec3& offset)
{
	position += offset;
	recalculate();
}

void Camera::add_rotation(const float x, const float y)
{
	rotation.x += x;
	rotation.y += y;
	recalculate();
}

void Camera::move_backward(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}


void Camera::move_down(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}


void Camera::move_forward(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}


void Camera::move_left(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}


void Camera::move_right(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}


void Camera::move_up(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}

void Camera::recalculate()
{
	rotation.x = MathUtil::clamp_float(rotation.x, glm::radians(-90.0f),
		glm::radians(90.0f));

	rotation.y = fmodf(rotation.y, TWO_PI);
	rotation.y = fmodf(rotation.y + TWO_PI, TWO_PI);

	view_matrix = glm::mat4(1.0f);
	view_matrix = glm::rotate(view_matrix, rotation.x, 
		glm::vec3(1.0f, 0.0f, 0.0f));
	view_matrix = glm::rotate(view_matrix, rotation.y, 
		glm::vec3(0.0f, 1.0f, 0.0f));
	view_matrix = glm::translate(view_matrix, -position);

	inverse_view_matrix = glm::inverse(view_matrix);
}

void Camera::set_position(const float x, const float y, const float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
	recalculate();
}

void Camera::set_rotation(const float x, const float y)
{
	rotation.x = x;
	rotation.y = y;
	recalculate();
}