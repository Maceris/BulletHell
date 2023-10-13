#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace ImageConverter
{
	/// <summary>
	/// Convert an image from the given source file, and output it 
	/// to the given destination.
	/// </summary>
	/// <param name="source"></param>
	/// <param name="destination"></param>
	void convert_image(const fs::directory_entry& source);
}