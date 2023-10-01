#pragma once

#include <filesystem>

namespace fs = std::filesystem;
/// <summary>
/// The maximum number of bone weights that can affect a vertex, the
/// default value used by Assimp when limiting bone weights.
/// </summary>
constexpr auto MAX_WEIGHTS = 4;

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