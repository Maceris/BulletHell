#pragma once

#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

/// <summary>
/// Check if an array contains an element.
/// </summary>
/// <typeparam name="T">The type of the array.</typeparam>
/// <typeparam name="S">The size of the array.</typeparam>
/// <param name="list">The list to look through.</param>
/// <param name="element">The element to search for.</param>
/// <returns>Whether the container includes the given element.</returns>
template<typename T, std::size_t S>
bool constexpr contains(const T (& list)[S], 
	const T& element)
{
	return std::find(std::begin(list), std::end(list), element) 
		!= std::end(list);
};

/// <summary>
/// Change the file extension to the specified one. Returns a new string
/// instead of modifying the passed one in place.
/// </summary>
/// <param name="file_name">The file name, including extension.</param>
/// <param name="new_extension">The new file extension we want to have.</param>
/// <returns>The new file name.</returns>
std::string constexpr swap_extension(const std::string& file_name,
	const std::string& new_extension)
{
	std::string result(file_name);
	if (result.empty())
	{
		return result;
	}
	const auto dot = result.find_last_of("\\.");
	if (dot == std::string::npos)
	{
		return result;
	}
	const size_t extension_size = result.size() - dot;
	result.erase(dot, extension_size);
	result += new_extension;
	return result;
}

/// <summary>
/// Utilities for interacting with the file system.
/// </summary>
class FileUtils
{
public:
	/// <summary>
	/// Process all assets.
	/// </summary>
	/// <returns>0 on success, nonzero if there are errors.</returns>
	static int process_all_files();
	
private:
	/// <summary>
	/// Process a file and output it ot the temp directoryu. Some files will
	/// be processed based on the extension, by default they are just copied.
	/// </summary>
	/// <param name="file">The file to process.</param>
	/// <returns>0 on success, nonzero if there are errors.</returns>
	static int process_file(const fs::directory_entry& file);

	/// <summary>
	/// Create the temp folder if it doesn't exist.
	/// </summary>
	static int create_temp_folder();

	/// <summary>
	/// Zip up everything in the temp folder into the output file.
	/// </summary>
	static int zip_contents();

	/// <summary>
	/// Delete the temporary folder and everything inside it.
	/// </summary>
	static void delete_temp_folder();
};