#pragma once

#include <filesystem>

class FileUtils
{
public:
	static int create_temp_folder();
	static int process_all_files();
	static int zip_contents();
	static void delete_temp_folder();
private:
	static int process_file(const std::filesystem::directory_entry& file);
	/// <summary>
	/// The folder name where assets are stored. 
	/// </summary>
	static const std::string asset_folder;
	static const std::string temp_folder;
	static const std::string output_file;
};