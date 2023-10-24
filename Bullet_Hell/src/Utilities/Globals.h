#pragma once

#define WIN32_LEAN_AND_MEAN	
#define GLM_FORCE_CTOR_INIT

#ifdef _DEBUG
#define DEBUG 1
#endif

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
