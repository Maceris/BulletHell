#pragma once

#include "Globals.h"

/// <summary>
/// Debugging related UI.
/// </summary>
class DebugUI
{
public:
	/// <summary>
	/// Draw all of the debug UI.
	/// </summary>
	static void draw();

	/// <summary>
	/// Draw a window for debugging information.
	/// </summary>
	static void draw_window_debug();

	/// <summary>
	/// Draw a window for profiling timer information.
	/// </summary>
	static void draw_window_timing();

	/// <summary>
	/// Process any inputs to the UI.
	/// </summary>
	static void handle_input();
private:
	/// <summary>
	/// Whether we should show the debug window.
	/// </summary>
	static bool show_debug_window;

	/// <summary>
	/// Whether we should show the window for profiling timers.
	/// </summary>
	static bool show_timing_window;

	/// <summary>
	/// Whether we should render in wireframe mode.
	/// </summary>
	static bool wireframe;
};