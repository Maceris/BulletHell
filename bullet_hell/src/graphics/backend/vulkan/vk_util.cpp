#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include "debugging/logger.h"
#include "graphics/backend/vulkan/global_state.h"
#include "graphics/backend/vulkan/vk_util.h"

void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties, VkBuffer& buffer,
	VkDeviceMemory& buffer_memory)
{
	VkBufferCreateInfo buffer_info{};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = usage;
	buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkDevice device = g_vk_state.device.logical;

	if (vkCreateBuffer(device, &buffer_info, nullptr,
		&buffer) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to create buffer");
	}

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

	VkMemoryAllocateInfo alloc_info{};
	alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize = memory_requirements.size;
	alloc_info.memoryTypeIndex = 
		find_memory_type(memory_requirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
	{
		LOG_FATAL("Failed to allocate buffer memory");
	}

	vkBindBufferMemory(device, buffer, buffer_memory, 0);
}

uint32_t find_memory_type(uint32_t type_filter,
	VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(g_vk_state.device.physical,
		&memory_properties);

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
	{
		if (type_filter & (1 << i) && (
			memory_properties.memoryTypes[i].propertyFlags & properties
			) == properties)
		{
			return i;
		}
	}
	LOG_FATAL("Failed to find suitable memory type");
	return 0;
}

#endif