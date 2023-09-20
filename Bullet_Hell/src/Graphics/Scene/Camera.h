#pragma once

#include "Globals.h"

#include <numbers>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

/// <summary>
/// Represents the camera in a scene.
/// </summary>
class Camera
{
public:
	/// <summary>
	/// The inverse of the view matrix, which is cached for convenience.
	/// </summary>
	glm::mat4 inverse_view_matrix;

	/// <summary>
	/// The position of the camera in world units.
	/// </summary>
	glm::vec3 position;

	/// <summary>
	/// The rotation of the camera, in radians. x represents pitch, and y 
	/// represents yaw. There are no controls for roll, so we leave out the 
	/// third coordinate.
	/// </summary>
	glm::vec2 rotation;

	/// <summary>
	/// The view matrix that combines translation and rotation of the camera.
	/// </summary>
	glm::mat4 view_matrix;

	/// <summary>
	/// Create a camera with default values.
	/// </summary>
	Camera();

	/// <summary>
	/// Copy a camera.
	/// </summary>
	Camera(const Camera&);

	/// <summary>
	/// Copy a camera.
	/// </summary>
	Camera& operator=(const Camera&);

	/// <summary>
	/// Clean up data.
	/// </summary>
	~Camera();

	/// <summary>
	/// Add rotation on top of the current rotation.
	/// </summary>
	/// <param name="x">The change in pitch (up and down), in radians.</param>
	/// <param name="y">The change in yaw (left and right), in radians.</param>
	void add_rotation(const float x, const float y);

	/// <summary>
	/// Move backward by the given increment, in world units.
	/// </summary>
	/// <param name="inc">The amount to move by.</param>
	void move_backward(const float inc);

	/// <summary>
	/// Move down by the given increment, in world units.
	/// </summary>
	/// <param name="inc">The amount to move by.</param>
	void move_down(const float inc);

	/// <summary>
	/// Move forward by the given increment, in world units.
	/// </summary>
	/// <param name="inc">The amount to move by.</param>
	void move_forward(const float inc);

	/// <summary>
	/// Move left by the given increment, in world units.
	/// </summary>
	/// <param name="inc">The amount to move by.</param>
	void move_left(const float inc);

	/// <summary>
	/// Move right by the given increment, in world units.
	/// </summary>
	/// <param name="inc">The amount to move by.</param>
	void move_right(const float inc);

	/// <summary>
	/// Move up by the given increment, in world units.
	/// </summary>
	/// <param name="inc">The amount to move by.</param>
	void move_up(const float inc);

	/// <summary>
	/// Set the position of the camera in world units.
	/// </summary>
	/// <param name="x">The new x position.</param>
	/// <param name="y">The new y position.</param>
	/// <param name="z">The new z position.</param>
	void set_position(const float x, const float y, const float z);

	/// <summary>
	/// Set the rotation of the camera.
	/// </summary>
	/// <param name="x">The new pitch (up and down), in radians.</param>
	/// <param name="y">The new yaw (left and right), in radians.</param>
	void set_rotation(const float x, const float y);

private:
	/// <summary>
	/// Constrain the position and rotation vectors to reasonable values, 
	/// then calculate the view and inverse view matrices.
	/// </summary>
	void recalculate();

	/// <summary>
	/// A constant value used to clamp rotation, precomputed for convenience.
	/// </summary>
	static inline constexpr float TWO_PI = 2 * std::numbers::pi;
};