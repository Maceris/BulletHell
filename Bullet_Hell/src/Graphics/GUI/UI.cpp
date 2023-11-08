#include "Graphics/GUI/UI.h"

#include "imgui.h"

#if DEBUG
#include "Graphics/GUI/DebugUI.h"
#endif

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