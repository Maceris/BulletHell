#pragma once

#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <atomic>
#include <vector>

#include <vulkan/vulkan.h>

/// <summary>
/// Handles the lifecycle of a swap chain.
/// </summary>
struct SwapChain
{
	VkSwapchainKHR current_swap_chain = VK_NULL_HANDLE;
	std::vector<VkImage> images{};
	std::vector<VkImageView> image_views{};
	VkExtent2D extent{0, 0};
	VkFormat image_format = VK_FORMAT_UNDEFINED;
	VkSwapchainKHR last_swap_chain = VK_NULL_HANDLE;

	std::vector<VkFramebuffer> frame_buffers{};
};

#endif
