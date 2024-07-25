#pragma once

//NOTE(ches) Make sure the debug flag gets carried over to the format we want
#ifdef _DEBUG
#define DEBUG 1
#endif

template <typename T>
void safe_delete(T* &ptr)
{
	delete ptr;
	ptr = nullptr;
}

template <typename T>
void safe_delete_array(T* &arr)
{
	delete[] arr;
	arr = nullptr;
}

#ifdef DEBUG
#define ALLOC new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#define ALLOC new
#endif
