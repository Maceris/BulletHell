#pragma once

#include "Globals.h"

#include <string>
#include <vector>

#include "glm/mat4x4.hpp"

/// <summary>
/// A column-major 4x4 matrix of floats.
/// </summary>
using BoneMatrix = glm::mat4;

/// <summary>
/// The maximum number of bone weights that can affect a vertex.
/// </summary>
constexpr auto MAX_WEIGHTS_PER_BONE = 4;

/// <summary>
/// A single key frame in an animation, which defines transformations to
/// apply to each bone.
/// </summary>
struct AnimatedFrame
{
	/// <summary>
	/// The bone transformations.
	/// </summary>
	std::vector<BoneMatrix> bone_matrices;

	/// <summary>
	/// The position in the bone transformation matrices buffer.
	/// </summary>
	unsigned int offset;

	AnimatedFrame()
		: offset{ 0 }
	{}

	AnimatedFrame(const AnimatedFrame&) = default;
	AnimatedFrame& operator=(const AnimatedFrame&) = default;
	~AnimatedFrame() = default;
};

/// <summary>
/// The bones that affect a vertex. There are MAX_WEIGHTS_PER_BONE
/// entries, but entries may be zero if not required. The indices line up, 
/// so weights[0] corresponds to indices[0].
/// </summary>
struct BoneWeights
{
	/// <summary>
	/// The weights of the bones that affect the vertex.
	/// </summary>
	float weights[MAX_WEIGHTS_PER_BONE];
	/// <summary>
	/// The index of the bones that affect the vertex.
	/// </summary>
	float indices[MAX_WEIGHTS_PER_BONE];
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
	std::vector<AnimatedFrame> frames;

	/// <summary>
	/// Create an animation.
	/// </summary>
	/// <param name="index">The name of the animation.</param>
	/// <param name="duration">The length of the animation, in ticks.
	/// </param>
	Animation(const std::string name, const double duration)
		: name{ name }
		, duration{ duration }
		, frames{}
	{}

	Animation(const Animation&) = default;
	Animation& operator=(const Animation&) = default;
	~Animation() = default;
}; 
