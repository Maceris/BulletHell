#include "packer_constants.h"

#include <format>

const std::string ASSET_FOLDER = "../../assets";
const std::string TEMP_FOLDER = "../../asset_temp";
const std::string OUTPUT_FILE = "./assets.zip";

const std::string IMAGE_FORMATS[8] =
{
    ".jpg", ".png", ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic"
};

const int MAX_BONES = 256;

const std::string ANIMATION_OUTPUT_EXTENSION = ".animation";
const std::string MATERIAL_OUTPUT_EXTENSION = ".material";
const std::string MODEL_OUTPUT_EXTENSION = ".model";
const std::string IMAGE_OUTPUT_EXTENSION = ".image";
