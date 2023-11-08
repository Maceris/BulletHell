#pragma once

#include "Globals.h"

#include <memory>

#include "Graphics/MouseInput.h"

/*
 * NOTE(ches) so we don't have to include glfw here, since it will interfere
 * with the windows definitions for things like APIENTRY.
 */
struct GLFWwindow;

/// <summary>
/// Used to create, track, and interact with an OpenGL window.
/// </summary>
class Window
{
public:
	/// <summary>
	/// Used to create and run a window.
	/// </summary>
	void initialize();

	/// <summary>
	/// Check if a key is pressed.
	/// </summary>
	/// <param name="key_code">The GLFW key code to look for.</param>
	/// <returns>Whether the provided key is pressed.</returns>
	bool is_key_pressed(int key_code);

	/// <summary>
	/// Render one frame.
	/// </summary>
	void render();

	/// <summary>
	/// Whether we should close the window, like if the user pressed the 
	/// close button on the window.
	/// </summary>
	bool should_close();

	/// <summary>
	/// Terminate the window and clean up.
	/// </summary>
	void terminate();
	
	/// <summary>
	/// Set up the object used to handle the OpenGL window.
	/// </summary>
	Window() = default;
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window() = default;

	/// <summary>
	/// The width of the window in pixels.
	/// </summary>
	unsigned int width;

	/// <summary>
	/// The height of the window in pixels.
	/// </summary>
	unsigned int height;

	/// <summary>
	/// Mouse input information for the window.
	/// </summary>
	MouseInput mouse_input;

	/// <summary>
	/// Used to configure the window before setting it up.
	/// </summary>
	struct WindowOptions
	{
		/// <summary>
		/// Whether anti-aliasing is enabled.
		/// </summary>
		bool anti_aliasing = true;

		/// <summary>
		/// Whether we want to use a compatible profile instead of the core
		/// one.
		/// </summary>
		bool compatible_profile = true;

		/// <summary>
		/// The maximum number of frames per second we are trying to render.
		/// </summary>
		unsigned int target_fps = 144;
		
		/// <summary>
		/// The desired height in pixels. If width and height are both set to
		/// 0, we will make the window full screen.
		/// </summary>
		unsigned int height = 0;

		/// <summary>
		/// The desired width in pixels. If width and height are both set to 0,
		/// we will make the window full screen.
		/// </summary>
		unsigned int width = 0;
	} options;

	/// <summary>
	/// The window handle.
	/// </summary>
	GLFWwindow* handle;
};
