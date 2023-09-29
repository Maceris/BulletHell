#pragma once

#include "Globals.h"

#include "glm.hpp"

#include "Animation.h"
#include "ModelLoader.h"

#include <memory>

/// <summary>
/// Tracks an animation and what frame we are on within it.
/// </summary>
class AnimationData
{
public:
	/// <summary>
	/// The ongoing animation.
	/// </summary>
	std::shared_ptr<Animation> current_animation;

	/// <summary>
	/// The index of the current frame in the animation.
	/// </summary>
	int current_frame_index;

	/// <summary>
	/// Set up animation data for an animation.
	/// </summary>
	/// <param name="animation">The animation.</param>
	AnimationData(std::shared_ptr<Animation> animation);

	/// <summary>
	/// Don't allow copy.
	/// </summary>
	AnimationData(const AnimationData&) = delete;

	/// <summary>
	/// Don't allow copy.
	/// </summary>
	AnimationData& operator=(const AnimationData&) = delete;
	
	/// <summary>
	/// Clean up.
	/// </summary>
	~AnimationData() = default;

	/// <summary>
	/// Fetch the current frame.
	/// </summary>
	/// <returns>The current frame from the animation.</returns>
	AnimatedFrame get_current_frame();

	/// <summary>
	/// Advance to the next frame in the animation, looping back to the start
	/// when we reach the end of the animation.
	/// </summary>
	void next_frame();

	/// <summary>
	/// Swap to a different animation and start at the beginning.
	/// </summary>
	/// <param name="animation">The new animation to use.</param>
	void set_current_animation(std::shared_ptr<Animation> animation);
};