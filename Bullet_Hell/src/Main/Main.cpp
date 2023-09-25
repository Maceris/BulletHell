#include "Main.h"

#include "Globals.h"

#include "Window.h"

#include <windows.h>
#include <iostream>

/// <summary>
/// The entry point to the program, and main loop.
/// </summary>
/// <returns>The exit code for the program.</returns>
int main()
{
    Window window = Window();
    window.initialize();
    
    while (!window.should_close())
    {
        window.render();
    }
    
    window.terminate();

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
