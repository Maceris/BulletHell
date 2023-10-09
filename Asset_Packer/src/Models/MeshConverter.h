#pragma once

#include <filesystem>

namespace fs = std::filesystem;

/// <summary>
/// The maximum number of bones that are allowed in a model.
/// </summary>
constexpr auto MAX_BONES = 150;

/// <summary>
/// The extension to use for the output files.
/// </summary>
extern const std::string OUTPUT_EXTENSION;

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