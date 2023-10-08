#pragma once

#include "Animation.h"

AnimatedFrame::AnimatedFrame(const std::vector<BoneMatrix>& bones_matrices)
	: bones_matrices(bones_matrices)
	, offset(0)
{}

Animation::Animation(const std::string name, const double duration,
	const std::vector<AnimatedFrame>& frames)
	: name(name)
	, duration(duration)
	, frames(frames)
{}

void Animation::append_weights_to_buffer(std::vector<float>& buffer)
{
	const int data_size = (int) (weights.size() * sizeof(BoneWeights));
	const float* data_start = (float*)weights.data();
	const float* data_end = data_start + data_size;

	buffer.insert(buffer.end(), data_start, data_end);
}

