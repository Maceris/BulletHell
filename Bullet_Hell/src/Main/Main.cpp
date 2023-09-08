#include "Main.h"

#include <windows.h>
#include <iostream>

#include "../Graphics/Window.h"

int main() {
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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    return main();
}
