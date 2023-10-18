#pragma once

#pragma region Scene constants
/// <summary>
/// The binding for the draw elements buffer SSBO.
/// </summary>
constexpr auto DRAW_ELEMENT_BINDING = 1;

/// <summary>
/// The binding for the model matrices buffer SSBO.
/// </summary>
constexpr auto MODEL_MATRICES_BINDING = 2;

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
/// The maximum number of point lights we allow in a scene.
/// </summary>
constexpr auto MAX_POINT_LIGHTS = 50;

/// <summary>
/// The maximum number of spot lights we allow in a scene.
/// </summary>
constexpr auto MAX_SPOT_LIGHTS = 50;
#pragma endregion

#pragma region Shadow constants
/// <summary>
/// The number of sections to split the frustum into.
/// </summary>
constexpr auto SHADOW_MAP_CASCADE_COUNT = 3;
#pragma endregion
