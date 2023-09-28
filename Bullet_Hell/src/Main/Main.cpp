#include "Globals.h"

#include "GameLogic.h"
#include "Logger.h"

#include <windows.h>
#include <iostream>

/// <summary>
/// The entry point to the program, and main loop.
/// </summary>
/// <returns>The exit code for the program.</returns>
int main()
{
    Logger::init();

    GameLogic game_logic;
    if (!game_logic.initialize())
    {
        LOG_FATAL("Failed to initialize the game logic.");
        exit(EXIT_FAILURE);
    }
    game_logic.run_game();

    //TODO(ches) do we need to call this here or in a callback?
    game_logic.on_close();

    Logger::destroy();

    exit(EXIT_SUCCESS);
    return 0;
}

/// <summary>
/// The Windows entrypoint. Just calls main().
/// </summary>
/// <param name="hInstance">The handle to an instance, used to identify the
/// executable when it's loaded in memory.</param>
/// <param name="hPrevInstance">Always zero, ignored.</param>
/// <param name="lpCmdLine">The command line arguments as a Unicode
/// string.</param>
/// <param name="nCmdShow">A flag indicating whether the main application
/// window is minimized, maxmized, or shown normally.</param>
/// <returns>The exit code for the program.</returns>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine, int nCmdShow)
{
    return main();
}
