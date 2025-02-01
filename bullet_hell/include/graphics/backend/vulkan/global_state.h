#pragma once

#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include <atomic>

#include "graphics/backend/vulkan/device.h"
#include "graphics/backend/vulkan/swap_chain.h"

#include <vulkan/vulkan.h>

#if _DEBUG
constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

/// <summary>
/// The validation layers that we want to enable.
/// </summary>
const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

struct WindowState
{
    std::atomic<int> height;
    std::atomic<int> width;
    std::atomic_bool resized = false;

    VkSurfaceKHR surface = nullptr;
    VkSurfaceFormatKHR surface_format;
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
};

struct VulkanState
{
    VkInstance instance{};
    VkDebugUtilsMessengerEXT debug_messenger{};

    Device device;
    SwapChain swap_chain;
    WindowState window_state;
};

/// <summary>
/// A global reference to the vulkan state.
/// </summary>
extern VulkanState g_vk_state;

#endif
