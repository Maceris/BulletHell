#pragma once

#include "Globals.h"

#include "mat4x4.hpp"

#include <string>
#include <vector>

/// <summary>
/// A single key frame in an animation, which defines transformations to
/// apply to each bone.
/// </summary>
struct AnimatedFrame
{
	/// <summary>
	/// The bone transformations.
	/// </summary>
	std::vector<glm::mat4> bones_matrices;

	/// <summary>
	/// The position in the list of bone transformation matrices.
	/// </summary>
	unsigned int offset;

	/// <summary>
	/// Create a new animated frame, given the transformation matrices.
	/// </summary>
	/// <param name="bones_matrices">The bone transformation matrices
	/// </param>
	AnimatedFrame(std::vector<glm::mat4> bones_matrices)
		: bones_matrices(bones_matrices)
		, offset(0)
	{}

	/// <summary>
	/// Copy an animated frame.
	/// </summary>
	AnimatedFrame(const AnimatedFrame&) = default;

	/// <summary>
	/// Copy assignment.
	/// </summary>
	AnimatedFrame& operator=(const AnimatedFrame&) = default;

	/// <summary>
	/// Clean up the frame.
	/// </summary>
	~AnimatedFrame() = default;
};

/// <summary>
/// A named animation.
/// </summary>
struct Animation
{
	/// <summary>
	/// The name of the animation.
	/// </summary>
	const std::string name;

	/// <summary>
	/// The length of the animation, in ticks.
	/// </summary>
	const double duration;

	/// <summary>
	/// The frames that make up the animation.
	/// </summary>
	const std::vector<AnimatedFrame> frames;

	/// <summary>
	/// Create an animation.
	/// </summary>
	/// <param name="name">The name of the animation.</param>
	/// <param name="duration">The length of the animation, in ticks.
	/// </param>
	/// <param name="frames">The frames that make up the animation.</param>
	Animation(std::string name, double duration,
		std::vector<AnimatedFrame> frames)
		: name(name)
		, duration(duration)
		, frames(frames)
	{}

	/// <summary>
	/// Copy an animation.
	/// </summary>
	Animation(const Animation&) = default;

	/// <summary>
	/// Copy an animation.
	/// </summary>
	Animation& operator=(const Animation&) = default;

	/// <summary>
	/// Clean up the animation.
	/// </summary>
	~Animation() = default;
}; 
