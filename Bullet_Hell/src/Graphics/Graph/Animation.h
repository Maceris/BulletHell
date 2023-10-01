#pragma once

#include "Globals.h"

#include <vector>

#include "glm.hpp"

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
	std::vector<BoneMatrix> bones_matrices;

	/// <summary>
	/// The position in the list of bone transformation matrices.
	/// </summary>
	unsigned int offset;

	/// <summary>
	/// Create a new animated frame, given the transformation matrices.
	/// </summary>
	/// <param name="bones_matrices">The bone transformation matrices
	/// </param>
	AnimatedFrame(const std::vector<BoneMatrix>& bones_matrices);

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
	/// The index of the animation.
	/// </summary>
	const int index;

	/// <summary>
	/// The length of the animation, in ticks.
	/// </summary>
	const double duration;

	/// <summary>
	/// The frames that make up the animation.
	/// </summary>
	const std::vector<AnimatedFrame> frames;

	/// <summary>
	/// The weights and index of each bone that affects each vertex. There 
	/// is one set of bone weights for each vertex.
	/// </summary>
	const std::vector<BoneWeights> weights;

	/// <summary>
	/// Create an animation.
	/// </summary>
	/// <param name="index">The index of the animation.</param>
	/// <param name="duration">The length of the animation, in ticks.
	/// </param>
	/// <param name="frames">The frames that make up the animation.</param>
	Animation(const int index, const double duration,
		const std::vector<AnimatedFrame>& frames);

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

	/// <summary>
	/// Append the bone index/weight data to a buffer.
	/// </summary>
	/// <param name="buffer">The buffer to append to.</param>
	void append_weights_to_buffer(std::vector<float>& buffer);
}; 


