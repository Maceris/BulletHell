#include "graphics/frontend/backend_type.h"

#if BACKEND_CURRENT == BACKEND_VULKAN

#include "graphics/backend/vulkan/global_state.h"

VulkanState g_vk_state{};

#endif