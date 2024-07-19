#pragma once

#include <cstdint>

using RenderConfig = uint32_t;

namespace RenderConfigValues
{
    constexpr RenderConfig ERROR_MASK = 1 << 31;
    constexpr RenderConfig ANIMATION_PASS_MASK = 1 << 30;
    constexpr RenderConfig SHADOW_PASS_MASK = 1 << 29;
    constexpr RenderConfig SCENE_PASS_MASK = 1 << 28;
    constexpr RenderConfig LIGHTING_PASS_MASK = 1 << 27;
    constexpr RenderConfig SKYBOX_PASS_MASK = 1 << 26;
    constexpr RenderConfig FILTER_PASS_MASK = 1 << 25;
    constexpr RenderConfig DEBUG_PASS_MASK = 1 << 24;
    constexpr RenderConfig GUI_PASS_MASK = 1 << 23;
    constexpr RenderConfig TRANSPARENCY_PASS_MASK = 1 << 19;
    constexpr RenderConfig SCENE_WIREFRAME_MASK = 1 << 15;

    constexpr RenderConfig SCENE_ENABLED_MASK = 
          SHADOW_PASS_MASK 
        | SCENE_PASS_MASK 
        | LIGHTING_PASS_MASK;
}
