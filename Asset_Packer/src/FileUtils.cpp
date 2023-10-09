#include "FileUtils.h"

#include <string>

#include "ZipFile.h"

#include "Logger.h"
#include "MeshConverter.h"

const std::string ASSET_FOLDER = "Assets";
const std::string TEMP_FOLDER = "Asset_Temp";
const std::string OUTPUT_FILE = "dist/assets.zip";

int FileUtils::process_all_files()
{
    if (!fs::exists(ASSET_FOLDER)) {
        LOG_FATAL("Asset directory " 
            + fs::absolute(ASSET_FOLDER).string() + " does not exist!");
        return 1;
    }
    int total_files = 0;
    int errors = 0;

    // In case something went wrong last time
    delete_temp_folder();

    if (fs::exists(OUTPUT_FILE))
    {
        LOG_WARNING("The file " + OUTPUT_FILE 
            + " already exists, removing it and recreating.");
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
            LOG_INFO("Creating temp directory " + target_path.string());

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
            LOG_ERROR("Error processing the file "
                + fs::absolute(entry.path()).string());
            ++errors;
        }
    }
    LOG_INFO("Zipping contents...");
    zip_contents();
    LOG_INFO("Done zipping!");

    delete_temp_folder();

    LOG_INFO("Processed " + std::to_string(total_files) + " files.");
    LOG_INFO("There were " + std::to_string(errors) + " errors.");

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
        LOG_WARNING("Temporary folder " + fs::absolute(TEMP_FOLDER).string()
        + " already exists");
        return 0;
    }
    if (!fs::create_directory(TEMP_FOLDER))
    {
        LOG_FATAL("Failed to create temporary folder " 
            + fs::absolute(TEMP_FOLDER).string());
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

        LOG_INFO("Zipping " + file_to_zip + "...");

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
        LOG_INFO("Trying to delete temp folder " + TEMP_FOLDER
            + " but it does not exist.");
        return;
    }
    uintmax_t files_removed = fs::remove_all(TEMP_FOLDER);
    LOG_INFO("Deleted temporary folder " + TEMP_FOLDER
        + ", removing a total of " + std::to_string(files_removed)
        + " files/directories");
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

    if (".obj" == extension || ".fbx" == extension)
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