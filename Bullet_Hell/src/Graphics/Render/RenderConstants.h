#pragma once

#pragma region Scene constants
/// <summary>
/// The binding for the draw elements buffer SSBO.
/// </summary>
constexpr auto DRAW_ELEMENT_BINDING = 0;

/// <summary>
/// The binding for the model matrices buffer SSBO.
/// </summary>
constexpr auto MODEL_MATRICES_BINDING = 1;

/// <summary>
/// The maximum number of draw elements we can handle at once.
/// </summary>
constexpr auto MAX_DRAW_ELEMENTS = 300;

/// <summary>
/// The maximum number of entities we can handle at once.
/// </summary>
constexpr auto MAX_ENTITIES = 100;

/// <summary>
/// The maximum number of materials we can handle at once.
/// </summary>
constexpr auto MAX_MATERIALS = 30;

/// <summary>
/// The maximum number of textures we can handle at once.
/// </summary>
constexpr auto MAX_TEXTURES = 16;
#pragma endregion

#pragma region Light constants
/// <summary>
/// The binding for the spot light SSBO.
/// </summary>
constexpr auto POINT_LIGHT_BINDING = 0;

/// <summary>
/// The binding for the spot light SSBO.
/// </summary>
constexpr auto SPOT_LIGHT_BINDING = 1;

/// <summary>
/// How many lights of each type (spot, point) we currently support.
/// </summary>
constexpr auto MAX_LIGHTS_SUPPORTED = 1000;

/// <summary>
/// Position (vec3 + ignored), color (vec3), intensity (1), Attenuation
/// (3 + ignored), in that order.
/// </summary>
constexpr auto POINT_LIGHT_SIZE = 4 + 3 + 1 + 4;

/// <summary>
/// Position (vec3 + ignored), color (vec3), intensity (1), Attenuation
/// (3 + ignored), cone direction (vec3), cutoff (1) in that order.
/// </summary>
constexpr auto SPOT_LIGHT_SIZE = 4 + 3 + 1 + 4 + 3 + 1;
#pragma endregion

#pragma region Shadow constants
/// <summary>
/// The number of sections to split the frustum into.
/// </summary>
constexpr auto SHADOW_MAP_CASCADE_COUNT = 3;
#pragma endregion
