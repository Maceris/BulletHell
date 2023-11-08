#include "Graphics/Graph/AnimationResource.h"

#include "Debugging/Logger.h"
#include "Main/GameLogic.h"
#include "Utilities/Portability.h"

/// <summary>
/// What is the latest material file verison we support loading.
/// </summary>
constexpr auto MAX_SUPPORTED_ANIMATION_VERSION = 1;

size_t AnimationLoader::get_loaded_resource_size(char* raw_buffer,
	size_t raw_size)
{
	return sizeof(Animation);
}

std::string AnimationLoader::get_pattern()
{
	return "*.animation";
}

bool AnimationLoader::load_resource(char* raw_buffer, size_t raw_size,
	std::shared_ptr<ResourceHandle> handle)
{
	std::shared_ptr<AnimationExtraData> extra
		= std::shared_ptr<AnimationExtraData>(ALLOC AnimationExtraData());
	handle->set_extra(extra);

	return parse_animation(extra, raw_buffer, raw_size);
}

bool AnimationLoader::parse_animation(
	std::shared_ptr<AnimationExtraData> extra_data, char* raw_buffer,
	size_t raw_size)
{
	RawStream stream{};
	stream.data = reinterpret_cast<unsigned char*>(raw_buffer);
	stream.data_size = raw_size;

	const uint32_t magic_bytes = read_uint32(stream);
	if (magic_bytes != 0xC0DE0003)
	{
		LOG_ERROR("This material does not start with the expected file ID bytes");
		return false;
	}

	const uint16_t version = read_uint16(stream);
	
	if (version > MAX_SUPPORTED_ANIMATION_VERSION)
	{
		LOG_ERROR("Unsupported material version " + std::to_string(version));
		return false;
	}

	std::string name = read_string(stream);

	const uint64_t raw_duration = read_uint64(stream);
	const double duration = reinterpret_cast<const double&>(raw_duration);

	auto animation = std::make_shared<Animation>(name, duration);

	const uint64_t frame_count = read_uint64(stream);
	const uint64_t bones_size = read_uint64(stream);

	std::vector<AnimatedFrame> frames;

	for (size_t i = 0; i < frame_count; ++i)
	{
		AnimatedFrame frame;
		for (size_t j = 0; j < bones_size; ++j)
		{
			frame.bone_matrices.push_back(read_mat4(stream));
		}
		animation->frames.push_back(frame);
	}

	extra_data->animation = animation;

	return true;
}

std::shared_ptr<Animation> load_animation(const std::string& name)
{
	Resource resource(name);
	auto handle = g_game_logic->resource_cache->get_handle(&resource);
	std::shared_ptr<AnimationExtraData> model_extra =
		static_pointer_cast<AnimationExtraData>(handle->get_extra());
	return model_extra->animation;
}