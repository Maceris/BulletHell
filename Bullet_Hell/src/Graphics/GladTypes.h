#pragma once
/*
 * NOTE(ches) The purpose of this file is to define all the OpenGL types that
 * we need in headers, without having to include glad or glfw.
 * 
 * The reason for this is glad defines some things that conflicts with windows
 * includes and has to be declared after windows.
 * 
 * If we need these types in headers, however, it becomes very tricky to 
 * ensure that glad is always included last in other headers.
 */

typedef unsigned int GLuint;
typedef int GLint;
