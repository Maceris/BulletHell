#pragma once

#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <atomic>

#include "graphics/backend/vulkan/device.h"
#include "graphics/backend/vulkan/swap_chain.h"

#include <vulkan/vulkan.h>

struct WindowState
{
    std::atomic<int> height;
    std::atomic<int> width;
    std::atomic_bool resized = false;

    VkSurfaceKHR surface = nullptr;
    VkSurfaceFormatKHR* surface_format = nullptr;
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
};

struct VulkanState
{
    VkInstance instance{};
    VkDebugUtilsMessengerEXT debug_messenger{};

    SwapChainSupport swap_chain_support;

    Device device;
    SwapChain swap_chain;
    WindowState window_state;
};

/// <summary>
/// A global reference to the vulkan state.
/// </summary>
extern VulkanState g_vk_state;

#endif
