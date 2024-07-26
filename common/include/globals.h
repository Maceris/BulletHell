#pragma once

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

#ifdef _DEBUG
#define ALLOC new(_NORMAL_BLOCK,__FILE__, __LINE__)
#else
#define ALLOC new
#endif
