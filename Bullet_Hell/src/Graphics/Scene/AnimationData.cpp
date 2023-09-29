#include "AnimationData.h"
#include "Logger.h"

AnimationData::AnimationData(std::shared_ptr<Animation> animation)
	: current_animation(std::move(animation))
	, current_frame_index(0)
{
	LOG_ASSERT(current_animation != nullptr);
}

AnimatedFrame AnimationData::get_current_frame()
{
	LOG_ASSERT(current_animation.get() != nullptr);
	return current_animation->frames[current_frame_index];
}

void AnimationData::next_frame()
{
	LOG_ASSERT(current_animation.get() != nullptr);
	int next_frame = current_frame_index + 1;
	if (next_frame >= current_animation->frames.size())
	{
		current_frame_index = 0;
	}
	else
	{
		current_frame_index = next_frame;
	}
}

void AnimationData::set_current_animation(
	std::shared_ptr<Animation> animation)
{
	LOG_ASSERT(animation != nullptr);
	current_animation = std::move(animation);
}