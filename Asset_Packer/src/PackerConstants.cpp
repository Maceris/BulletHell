#include "PackerConstants.h"

#include <format>

const std::string ASSET_FOLDER = "Assets";
const std::string TEMP_FOLDER = "Asset_Temp";
const std::string OUTPUT_FILE = std::format("dist/{}/{}/assets.zip", 
    PLATFORM_NAME, CONFIGURATION_NAME);

const std::string IMAGE_FORMATS[8] =
{
    ".jpg", ".png", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic"
};

const int MAX_BONES = 256;

const std::string ANIMATION_OUTPUT_EXTENSION = ".animation";
const std::string MATERIAL_OUTPUT_EXTENSION = ".material";
const std::string MODEL_OUTPUT_EXTENSION = ".model";
const std::string IMAGE_OUTPUT_EXTENSION = ".image";
