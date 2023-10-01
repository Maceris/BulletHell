#include "FileUtils.h"

#include <string>
#include <iostream>

#include "ZipFile.h"

#include "MeshConverter.h"

const std::string ASSET_FOLDER = "Assets";
const std::string TEMP_FOLDER = "Asset_Temp";
const std::string OUTPUT_FILE = "dist/assets.zip";

int FileUtils::process_all_files()
{
    if (!fs::exists(ASSET_FOLDER)) {
        std::cout << "Asset directory " << fs::absolute(ASSET_FOLDER);
        std::cout << " does not exist!" << std::endl;
        return 1;
    }
    int total_files = 0;
    int errors = 0;

    // In case something went wrong last time
    delete_temp_folder();

    if (fs::exists(OUTPUT_FILE))
    {
        std::cout << "The file " << OUTPUT_FILE << " already exists, ";
        std::cout << "removing it and recreating." << std::endl;
        fs::remove(OUTPUT_FILE);
    }

    create_temp_folder();

    for (const auto& entry : fs::recursive_directory_iterator(ASSET_FOLDER))
    {
        if (entry.is_directory())
        {
            std::string target_folder_name = entry.path().string();
            target_folder_name = target_folder_name.erase(0, 
                ASSET_FOLDER.size());
            target_folder_name = TEMP_FOLDER + target_folder_name;

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
    if (fs::exists(TEMP_FOLDER))
    {
        std::cout << "WARNING: Temporary folder " << fs::absolute(TEMP_FOLDER);
        std::cout << " already exists" << std::endl;
        return 0;
    }
    if (!fs::create_directory(TEMP_FOLDER))
    {
        std::cout << "Failed to create temporary folder ";
        std::cout << fs::absolute(TEMP_FOLDER) << std::endl;
        return 1;
    }
    return 0;
}

int FileUtils::zip_contents()
{
    for (const auto& entry : fs::recursive_directory_iterator(TEMP_FOLDER))
    {
        if (!entry.is_regular_file())
        {
            continue;
        }

        std::string file_to_zip = entry.path().string();
        std::cout << "Zipping " << entry.path() << "..." << std::endl;

        std::string in_archive_name = file_to_zip;
        in_archive_name.erase(0, TEMP_FOLDER.size() + 1);

        ZipFile::AddFile(OUTPUT_FILE, file_to_zip, in_archive_name);
    }
   
    return 0;
}

void FileUtils::delete_temp_folder()
{
    if (!fs::exists(TEMP_FOLDER))
    {
        std::cout << "Trying to delete temp folder " << TEMP_FOLDER;
        std::cout << " but it does not exist." << std::endl;
        return;
    }
    uintmax_t files_removed = fs::remove_all(TEMP_FOLDER);
    std::cout << "Deleted temporary folder " << TEMP_FOLDER;
    std::cout << ", removing a total of " << files_removed;
    std::cout << " files/directories" << std::endl;
}

int FileUtils::process_file(const fs::directory_entry& file)
{
    std::string target_file_name = file.path().string();
    target_file_name = target_file_name.erase(0, ASSET_FOLDER.size());
    target_file_name = TEMP_FOLDER + target_file_name;

    fs::path target_path(target_file_name);

    std::string extension = target_file_name;
    auto dot = extension.find_last_of("\\.");
    extension = extension.substr(dot, extension.size());

    if (".obj" == extension)
    {
        MeshConverter::convert_model(file);
    }
    else
    {
        // Copy as a fallback
        if (!fs::exists(target_path))
        {
            fs::copy_file(file.path(), target_path);
        }
    }
    return 0;
}