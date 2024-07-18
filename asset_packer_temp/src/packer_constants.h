#pragma once

#include <string>

#pragma region File utils
/// <summary>
/// The folder name where assets are stored. 
/// </summary>
extern const std::string ASSET_FOLDER;

/// <summary>
/// The folder where assets are gathered during processing.
/// </summary>
extern const std::string TEMP_FOLDER;

/// <summary>
/// The zip file where we will output everything.
/// </summary>
extern const std::string OUTPUT_FILE;

/// <summary>
/// A list of image formats that we convert.
/// </summary>
extern const std::string IMAGE_FORMATS[8];
#pragma endregion

#pragma region Mesh converter
/// <summary>
/// The maximum number of bones that are allowed in a model.
/// </summary>
extern const int MAX_BONES;

/// <summary>
/// The file extension to use for animations.
/// </summary>
extern const std::string ANIMATION_OUTPUT_EXTENSION;

/// <summary>
/// The file extension to use for materials.
/// </summary>
extern const std::string MATERIAL_OUTPUT_EXTENSION;

/// <summary>
/// The file extension to use for models.
/// </summary>
extern const std::string MODEL_OUTPUT_EXTENSION;
#pragma endregion

#pragma region Image converter
extern const std::string IMAGE_OUTPUT_EXTENSION;
#pragma endregion