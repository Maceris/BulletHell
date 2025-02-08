#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include "debugging/logger.h"
#include "graphics/backend/vulkan/global_state.h"
#include "graphics/backend/vulkan/render_state.h"

VkCommandBuffer RenderState::current_command_buffer() const
{
	return command_buffers[current_frame];
}

namespace RenderUtil
{
	void create_command_buffers()
	{
		VkCommandPoolCreateInfo pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		pool_info.queueFamilyIndex =
			g_vk_state.device.indices.graphics_family.value();

		auto& device = g_vk_state.device.logical;

		if (vkCreateCommandPool(device, &pool_info, nullptr,
			&g_vk_state.render.command_pool) != VK_SUCCESS)
		{
			LOG_FATAL("Failed to create command pool");
		}

		g_vk_state.render.command_buffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocate_info.commandPool = g_vk_state.render.command_pool;
		allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

		if (vkAllocateCommandBuffers(device, &allocate_info,
			g_vk_state.render.command_buffers.data())
			!= VK_SUCCESS)
		{
			LOG_FATAL("Failed to create command buffer");
		}
	}

	void create_synchronization_objects()
	{
		VkSemaphoreCreateInfo semaphore_info{};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_info{};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		g_vk_state.render.image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
		g_vk_state.render.render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
		g_vk_state.render.frame_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

		auto& device = g_vk_state.device.logical;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			if (vkCreateSemaphore(device, &semaphore_info, nullptr,
				&g_vk_state.render.image_available_semaphores[i]) != VK_SUCCESS
				|| vkCreateSemaphore(device, &semaphore_info, nullptr,
					&g_vk_state.render.render_finished_semaphores[i]) != VK_SUCCESS
				|| vkCreateFence(device, &fence_info, nullptr,
					&g_vk_state.render.frame_in_flight_fences[i]) != VK_SUCCESS)
			{
				LOG_FATAL("Failed to create synchronization objects");
			}
		}
	}

	void destroy_command_buffers()
	{
		//NOTE(ches) Buffers get cleaned up with the pool.
		vkDestroyCommandPool(g_vk_state.device.logical,
			g_vk_state.render.command_pool, nullptr);
	}

	void destroy_synchronization_objects()
	{
		auto& device = g_vk_state.device.logical;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vkDestroySemaphore(device,
				g_vk_state.render.image_available_semaphores[i], nullptr);
			vkDestroySemaphore(device,
				g_vk_state.render.render_finished_semaphores[i], nullptr);
			vkDestroyFence(device,
				g_vk_state.render.frame_in_flight_fences[i], nullptr);
		}
	}
}

#endif