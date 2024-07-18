#include "FileUtils.h"
#include "Debugging/Logger.h"

const std::string version = "0.0.1";

/// <summary>
/// The entry point to the program, and main loop.
/// </summary>
/// <returns>The exit code for the program.</returns>
int main(int argc, char* argv[])
{
    Logger::init();
    Logger::set_display_flags("MODEL", FLAG_WRITE_TO_DEBUGGER);
    Logger::set_display_flags("IMAGE", FLAG_WRITE_TO_DEBUGGER);

    LOG_INFO("Asset packer v" + version);

    int result = FileUtils::process_all_files();

    Logger::destroy();
    return result;
}