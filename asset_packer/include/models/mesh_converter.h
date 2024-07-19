#pragma once

#include <filesystem>

namespace fs = std::filesystem;

namespace MeshConverter
{
	/// <summary>
	/// Convert a 3d model from the given source file, and output it 
	/// to the given destination.
	/// </summary>
	/// <param name="source"></param>
	/// <param name="destination"></param>
	void convert_model(const fs::directory_entry& source);
}