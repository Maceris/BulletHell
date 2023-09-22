#pragma once

#include <filesystem>

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
	static int process_file(const std::filesystem::directory_entry& file);

	/// <summary>
	/// The folder name where assets are stored. 
	/// </summary>
	static const std::string asset_folder;

	/// <summary>
	/// The folder where assets are gathered during processing.
	/// </summary>
	static const std::string temp_folder;

	/// <summary>
	/// The zip file where we will output everything.
	/// </summary>
	static const std::string output_file;

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