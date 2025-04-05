#pragma once

#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <cstdint>

#include <vulkan/vulkan.h>

void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties, VkBuffer& buffer,
	VkDeviceMemory& buffer_memory);

uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);

#endif