#include "Graphics/Scene/AnimationData.h"

#include "Debugging/Logger.h"
#include "Graphics/Graph/Animation.h"

AnimationData::AnimationData(std::shared_ptr<Animation> animation)
	: current_animation{ animation }
	, current_frame_index{ 0 }
{}

AnimatedFrame& AnimationData::get_current_frame()
{
	LOG_ASSERT(current_animation);
	return current_animation->frames[current_frame_index];
}

void AnimationData::next_frame()
{
	LOG_ASSERT(current_animation);
	int next_frame = current_frame_index + 1;
	if (next_frame >= current_animation->frames.size())
	{
		if (interrupted_animation)
		{
			current_animation = interrupted_animation;
			interrupted_animation = nullptr;
		}
		current_frame_index = 0;
	}
	else
	{
		current_frame_index = next_frame;
	}
}

void AnimationData::run_immediate_once(std::shared_ptr<Animation> animation)
{
	LOG_ASSERT(animation);
	if (animation == current_animation)
	{
		return;
	}
	if (interrupted_animation != nullptr)
	{
		interrupted_animation = current_animation;
	}
	current_animation = animation;
	current_frame_index = 0;
}

void AnimationData::set_current_animation(
	std::shared_ptr<Animation> animation)
{
	LOG_ASSERT(animation);
	if (interrupted_animation == nullptr && animation != current_animation)
	{
		current_animation = animation;
		current_frame_index = 0;
	}
	else if (animation != interrupted_animation)
	{
		interrupted_animation = animation;
	}
}