#include "FileUtils.h"

#include <string>
#include <iostream>

#include "ZipFile.h"

namespace fs = std::filesystem;

const std::string FileUtils::asset_folder = "Assets";
const std::string FileUtils::temp_folder = "Asset_Temp";
const std::string FileUtils::output_file = "dist/assets.zip";

int FileUtils::process_all_files()
{
    if (!fs::exists(asset_folder)) {
        std::cout << "Asset directory " << fs::absolute(asset_folder);
        std::cout << " does not exist!" << std::endl;
        return 1;
    }
    int total_files = 0;
    int errors = 0;

    // In case something went wrong last time
    delete_temp_folder();

    if (fs::exists(output_file))
    {
        std::cout << "The file " << output_file << " already exists, ";
        std::cout << "removing it and recreating." << std::endl;
        fs::remove(output_file);
    }

    create_temp_folder();

    for (const auto& entry : fs::recursive_directory_iterator(asset_folder))
    {
        if (entry.is_directory())
        {
            std::string target_folder_name = entry.path().string();
            target_folder_name = target_folder_name.erase(0, 
                asset_folder.size());
            target_folder_name = temp_folder + target_folder_name;

            std::filesystem::path target_path(target_folder_name);
            std::cout << "Creating temp directory " << target_path << std::endl;

            if (!fs::exists(target_path))
            {
                fs::create_directory(target_path);
            }
        }
        if (!entry.is_regular_file())
        {
            continue;
        }
        ++total_files;
        if (process_file(entry) != 0)
        {
            std::cout << "Error processing the file ";
            std::cout << fs::absolute(entry.path()) << std::endl;
            ++errors;
        }
    }
    std::cout << "Zipping contents..." << std::endl;
    zip_contents();
    std::cout << "Done zipping!" << std::endl;

    delete_temp_folder();

    std::cout << "Processed " << total_files << " files." << std::endl;
    std::cout << "There were " << errors << " errors." << std::endl;
    if (errors > 0)
    {
        return 1;
    }
    return 0;
}

int FileUtils::create_temp_folder()
{
    if (fs::exists(temp_folder))
    {
        std::cout << "WARNING: Temporary folder " << fs::absolute(temp_folder);
        std::cout << " already exists" << std::endl;
        return 0;
    }
    if (!fs::create_directory(temp_folder))
    {
        std::cout << "Failed to create temporary folder ";
        std::cout << fs::absolute(temp_folder) << std::endl;
        return 1;
    }
    return 0;
}

int FileUtils::zip_contents()
{
    for (const auto& entry : fs::recursive_directory_iterator(temp_folder))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::string file_to_zip = entry.path().string();
        std::cout << "Zipping " << entry.path() << "..." << std::endl;

        std::string in_archive_name = file_to_zip;
        in_archive_name.erase(0, temp_folder.size() + 1);

        ZipFile::AddFile(output_file, file_to_zip, in_archive_name);
    }
   
    return 0;
}

void FileUtils::delete_temp_folder()
{
    if (!fs::exists(temp_folder))
    {
        std::cout << "Trying to delete temp folder " << temp_folder;
        std::cout << " but it does not exist." << std::endl;
        return;
    }
    uintmax_t files_removed = fs::remove_all(temp_folder);
    std::cout << "Deleted temporary folder " << temp_folder;
    std::cout << ", removing a total of " << files_removed;
    std::cout << " files/directories" << std::endl;
}

int FileUtils::process_file(const fs::directory_entry& file)
{
    std::string target_file_name = file.path().string();
    target_file_name = target_file_name.erase(0, asset_folder.size());
    target_file_name = temp_folder + target_file_name;

    std::filesystem::path target_path(target_file_name);

    if (!fs::exists(target_path))
    {
        fs::copy_file(file.path(), target_path);
    }
    return 0;
}