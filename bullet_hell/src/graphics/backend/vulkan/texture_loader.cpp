#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include "debugging/logger.h"
#include "main/game_logic.h"
#include "graphics/backend/base/format_mapper.h"
#include "graphics/backend/vulkan/global_state.h"
#include "graphics/backend/vulkan/vk_util.h"
#include "graphics/frontend/texture.h"
#include "graphics/frontend/texture_loader.h"
#include "graphics/graph/texture_resource.h"
#include "resource_cache/resource_cache.h"

#include <vulkan/vulkan.h>

static_assert(sizeof(TextureHandle) >= sizeof(VkImage));
static_assert(sizeof(MemoryHandle) >= sizeof(VkDeviceMemory));

Texture TextureLoader::load(const char* buffer, ImageFormat format,
	uint32_t width, uint32_t height)
{
	VkBuffer staging_buffer;
	VkDeviceMemory staging_buffer_memory;

	VkDeviceSize size = width * height * 4;

	create_buffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		staging_buffer, staging_buffer_memory
	);

	VkDevice device = g_vk_state.device.logical;

	void* data;
	vkMapMemory(device, staging_buffer_memory, 0, size, 0, &data);
	memcpy(data, buffer, static_cast<size_t>(size));
	vkUnmapMemory(device, staging_buffer_memory);

	VkImage texture_image;
	VkDeviceMemory texture_image_memory;

	VkImageCreateInfo image_info{};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = width;
	image_info.extent.height = height;
	image_info.extent.depth = 1;
	image_info.mipLevels = 1;//TODO(ches) create mipmap
	image_info.arrayLayers = 1;
	image_info.format = FormatMapper::map(format);
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.flags = 0;

	if (vkCreateImage(device, &image_info, nullptr, &texture_image) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create image");
	}

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(device, texture_image, &memory_requirements);

	VkMemoryAllocateInfo alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = find_memory_type(
		memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	if (vkAllocateMemory(device, &alloc_info, nullptr, &texture_image_memory)
		!= VK_SUCCESS)
	{
		LOG_FATAL("Failed to allocate image memory");
	}

	vkBindImageMemory(device, texture_image, texture_image_memory, 0);

	return Texture{ 
		reinterpret_cast<TextureHandle>(texture_image), 
		reinterpret_cast<MemoryHandle>(texture_image_memory), 
		width, height 
	};
}

Texture TextureLoader::load(const std::string_view name)
{
	Resource resource(name);
	auto handle = g_game_logic->resource_cache->get_handle(&resource);
	std::shared_ptr<TextureExtraData> model_extra =
		static_pointer_cast<TextureExtraData>(handle->get_extra());
	return *(model_extra->texture);
}

#endif