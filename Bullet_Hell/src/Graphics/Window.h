#pragma once

#include <memory>

#include "glad.h"
#define GLFW_INCLUDE_NONE
#include "glfw3.h"

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

	/// <summary>
	/// Windows cannot be copied, we only ever use one.
	/// </summary>
	Window(const Window&) = delete;

	/// <summary>
	/// Windows cannot be assigned, we only ever use one.
	/// </summary>
	Window& operator=(const Window&) = delete;

	~Window() = default;

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
		unsigned int height = 480;

		/// <summary>
		/// The desired width in pixels. If width and height are both set to 0,
		/// we will make the window full screen.
		/// </summary>
		unsigned int width = 640;
	} options;

private:
	/// <summary>
	/// The window handle.
	/// </summary>
	GLFWwindow* window;

	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;

	// 
	/// <summary>
	/// The width of the window in pixels.
	/// </summary>
	unsigned int width;

	/// <summary>
	/// The height of the window in pixels.
	/// </summary>
	unsigned int height;
};
