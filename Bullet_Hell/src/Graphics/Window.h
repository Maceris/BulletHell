#pragma once

#include <memory>

#include "glad.h"
#define GLFW_INCLUDE_NONE
#include "glfw3.h"

// Used to create, track, and interact with an OpenGL window
class Window
{
public:
	// Used to create and run a window.
	void initialize();

	// Render one frame
	void render();

	// Whether we should close the window
	bool should_close();

	// Terminate the window and clean up
	void terminate();
	
	// Set up the object used to handle the OpenGL window
	Window() = default;

	// Windows cannot be copied, there is only ever one
	Window(const Window&) = delete;

	// Windows cannot be assigned, there is only ever one
	Window& operator=(const Window&) = delete;

	~Window() = default;

	// Used to configure the window before setting it up
	struct WindowOptions
	{
		// Whether anti-aliasing is enabled
		bool anti_aliasing = true;

		// Whether we want to use a compatible profile instead of the core one
		bool compatible_profile = true;

		// The maximum number of frames per second we are trying to render
		unsigned int target_fps = 144;
		
		/*
		The desired height in pixels. If width and height are both set to 0, 
		we will make the window full screen.
		*/
		unsigned int height = 480;

		/*
		The desired width in pixels. If width and height are both set to 0,
		we will make the window full screen.
		*/
		unsigned int width = 640;
	} options;

private:
	// The window handle
	GLFWwindow* window;
	GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;

	// The width of the window in pixels
	unsigned int width;
	// The height of the window in pixels
	unsigned int height;
};
