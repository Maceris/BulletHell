#include <windows.h>

#include <iostream>

#include "debugging/logger.h"
#include "debugging/timer.h"
#include "main/game_logic.h"

GameLogic* g_game_logic = nullptr;

/// <summary>
/// The entry point to the program, and main loop.
/// </summary>
/// <returns>The exit code for the program.</returns>
int main()
{
    Logger::init();
    Logger::set_display_flags("Event", FLAG_WRITE_TO_DEBUGGER);
    Logger::set_display_flags("Event Loop", 0);

    TIME_START("Game Init");

    g_game_logic = ALLOC GameLogic();

    if (!g_game_logic->initialize())
    {
        LOG_FATAL("Failed to initialize the game logic.");
        exit(EXIT_FAILURE);
    }
    TIME_END("Game Init");

    g_game_logic->run_game();

    g_game_logic->on_close();

    Logger::destroy();

    exit(EXIT_SUCCESS);
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
