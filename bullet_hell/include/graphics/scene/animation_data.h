#pragma once

#include "globals.h"

#include <memory>

struct Animation;
struct AnimatedFrame;

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

	AnimationData(const AnimationData&) = delete;
	AnimationData& operator=(const AnimationData&) = delete;
	~AnimationData() = default;

	/// <summary>
	/// Fetch the current frame.
	/// </summary>
	/// <returns>The current frame from the animation.</returns>
	AnimatedFrame& get_current_frame();

	/// <summary>
	/// Advance to the next frame in the animation, looping back to the start
	/// when we reach the end of the animation.
	/// </summary>
	void next_frame();

	/// <summary>
	/// Start using a specific animation until it ends, then go back to
	/// whatever we had been animating. If a one-time animation is already
	/// running, it will be replaced with this and once we complete an
	/// immediate animation then we will return to the animation that was
	/// first interrupted.
	/// </summary>
	/// <param name="animation">The animation to run.</param>
	void run_immediate_once(std::shared_ptr<Animation> animation);

	/// <summary>
	/// Swap to a different animation and start at the beginning. If we are
	/// currently running an immediate animation, this will set the animation
	/// to return to once that is done.
	/// </summary>
	/// <param name="animation">The new animation to use.</param>
	void set_current_animation(std::shared_ptr<Animation> animation);

private:
	/// <summary>
	/// The animation that we had been using until an immediate one was run.
	/// </summary>
	std::shared_ptr<Animation> interrupted_animation = nullptr;
};