#pragma once

#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <atomic>
#include <vector>

#include <vulkan/vulkan.h>

struct RenderState
{
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> frame_in_flight_fences;
	std::atomic_bool rendering_active = true;
	uint32_t current_frame = 0;

	std::vector<VkCommandBuffer> command_buffers;
	VkCommandPool command_pool;

	VkCommandBuffer current_command_buffer() const;
};

namespace RenderUtil
{
	void create_command_buffers();
	void create_synchronization_objects();
	void destroy_command_buffers();
	void destroy_synchronization_objects();
}

#endif