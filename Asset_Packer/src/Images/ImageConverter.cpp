#include "ImageConverter.h"

#include "FileUtils.h"
#include "Logger.h"
#include "Portability.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const std::string IMAGE_OUTPUT_EXTENSION = ".image";

void ImageConverter::convert_image(const fs::directory_entry& source)
{
	std::string target_file_name = source.path().string();
	LOG_TAGGED("IMAGE", "Converting image " + target_file_name);
	
	int width = 0;
	int height = 0;
	int actual_channels;
	const int desired_channels = 4; //NOTE(ches) RGBA
	unsigned char* results = stbi_load(target_file_name.c_str(), &width,
		&height, &actual_channels, desired_channels);

	LOG_ASSERT(results != nullptr);
	if (results == nullptr)
	{
		std::string reason(stbi_failure_reason());
		LOG_TAGGED("IMAGE", "Failed to load image because " + reason);
		return;
	}

	//NOTE(ches) swap folder names
	target_file_name.erase(0, ASSET_FOLDER.size());
	target_file_name = TEMP_FOLDER + target_file_name;
	
	//NOTE(ches) swap extensions
	auto dot = target_file_name.find_last_of("\\.");
	const int extension_size = target_file_name.size() - dot;
	target_file_name.erase(dot, extension_size);
	target_file_name += IMAGE_OUTPUT_EXTENSION;

	std::ofstream out(target_file_name, std::ios::out | std::ios::binary
		| std::ios::app);
	out.imbue(std::locale::classic());

	write_uint32(width, out);
	write_uint32(height, out);
	write_data_array(results, width * height * desired_channels, out);
	out.close();
}
