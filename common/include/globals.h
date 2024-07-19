#pragma once

//NOTE(ches) Make sure the debug flag gets carried over to the format we want
#ifdef _DEBUG
#define DEBUG 1
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
