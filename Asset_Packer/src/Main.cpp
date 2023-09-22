#include <iostream>
#include <windows.h>

#include "FileUtils.h"

const std::string version = "0.0.1";

/// <summary>
/// The entry point to the program, and main loop.
/// </summary>
/// <returns>The exit code for the program.</returns>
int main(int argc, char* argv[])
{
    std::cout << "Asset packer v" << version << std::endl;

    int result = FileUtils::process_all_files();
    return result;
}