#include "MeshConverter.h"

#include "FileUtils.h"

#include <iostream>

const std::string OUTPUT_EXTENSION = ".model";

void MeshConverter::convert_model(const fs::directory_entry& source)
{
    std::string target_file_name = source.path().string();
    target_file_name = target_file_name.erase(0, ASSET_FOLDER.size());
  
    // Swap extensions
    auto dot = target_file_name.find_last_of("\\.");
    const int extension_size = target_file_name.size() - dot;
    target_file_name = target_file_name.erase(dot, extension_size);
    target_file_name = target_file_name + OUTPUT_EXTENSION;
    
    target_file_name = TEMP_FOLDER + target_file_name;

    fs::path target_path(target_file_name);
    
    
}