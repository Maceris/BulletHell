#include "TextureResource.h"

#include "Portability.h"

unsigned int TextureLoader::get_loaded_resource_size(char* raw_buffer,
	unsigned int raw_size)
{
	return sizeof(Texture);
}

std::string TextureLoader::get_pattern()
{
	return "*.image";
}

bool TextureLoader::load_resource(char* raw_buffer, unsigned int raw_size,
	std::shared_ptr<ResourceHandle> handle)
{
	std::shared_ptr<TextureExtraData> extra
		= std::shared_ptr<TextureExtraData>(ALLOC TextureExtraData());
	handle->set_extra(extra);

	return parse_texture(extra, raw_buffer, raw_size);
}

bool TextureLoader::parse_texture(std::shared_ptr<TextureExtraData> extra_data,
	char* raw_buffer, unsigned int raw_size)
{
	uint64_t size = 0;
	RawStream stream{};
	stream.data = reinterpret_cast<unsigned char*>(raw_buffer);
	stream.data_size = raw_size;

	uint32_t width = read_uint32(stream);
	uint32_t height = read_uint32(stream);

	unsigned char* image_data = nullptr;
	read_data_array(stream, image_data, &size);

	if (size <= 0)
	{
		LOG_WARNING("Image does not contain data");
		return false;
	}
	extra_data->texture = std::make_shared<Texture>(width, height, 
		reinterpret_cast<const char*>(image_data));

	SAFE_DELETE_ARRAY(image_data);
	return true;
}
