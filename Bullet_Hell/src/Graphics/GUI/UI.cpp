#include "UI.h"

#if DEBUG
#include "DebugUI.h"
#endif

#include "imgui.h"

void UI::draw()
{
#if DEBUG
	DebugUI::draw();
#endif
}

void UI::handle_input()
{
#if DEBUG
	DebugUI::handle_input();
#endif
}