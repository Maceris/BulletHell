#pragma once

//NOTE(ches) Prevent clutter being pulled in
#define WIN32_LEAN_AND_MEAN
//NOTE(ches) Forcibly initialize all glm vectors/matrices to 0 on construction
#define GLM_FORCE_CTOR_INIT
//NOTE(ches) Prevent windows from interfering with glm
#define NOMINMAX

//NOTE(ches) Make sure the debug flag gets carried over to the format we want
#ifdef _DEBUG
#define DEBUG 1
#endif

//NOTE(ches) prevent the GLFW header from including OpenGL headers
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif

#if !defined(SAFE_DELETE)
#define SAFE_DELETE(ptr) if(ptr) delete ptr; ptr=nullptr;
#endif

#if !defined(SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(arr) if (arr) delete [] arr; arr=nullptr; 
#endif

#ifdef DEBUG
#define ALLOC new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#define ALLOC new
#endif
