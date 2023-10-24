#include "Camera.h"

#include "MathUtil.h"

Camera::Camera()
	: position(glm::vec3(0))
	, rotation(glm::vec2(0))
	, view_matrix(glm::mat4(1.0f))
	, inverse_view_matrix(glm::inverse(view_matrix))
{}

Camera::Camera(const Camera&) = default;
Camera& Camera::operator=(const Camera&) = default;
Camera::~Camera() = default;

void Camera::add_rotation(const float x, const float y)
{
	rotation.x += x;
	rotation.y += y;
	recalculate();
}

void Camera::move_backward(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(0, 0, 1, 0);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}


void Camera::move_down(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(0, -1, 0, 0);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}


void Camera::move_forward(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(0, 0, -1, 0);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}


void Camera::move_left(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(-1, 0, 0, 0);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}


void Camera::move_right(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(1, 0, 0, 0);
	temp *= inc;
	position += glm::vec3(temp);
	recalculate();
}


void Camera::move_up(const float inc)
{
	glm::vec4 temp = inverse_view_matrix * glm::vec4(0, 1, 0, 0);
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
	view_matrix = glm::rotate(view_matrix, rotation.x, glm::vec3(1, 0, 0));
	view_matrix = glm::rotate(view_matrix, rotation.y, glm::vec3(0, 1, 0));
	view_matrix = glm::translate(view_matrix, position);

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