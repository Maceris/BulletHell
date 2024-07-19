#pragma once

#include <type_traits>

/// <summary>
/// A template for specifying iterators over enums for range-based for loops.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <typeparam name="first_value"></typeparam>
/// <typeparam name="last_value"></typeparam>
template <typename EnumType, EnumType first_value, EnumType last_value>
class Iterator
{
private:
	typedef typename std::underlying_type_t<EnumType> underlying_type;
	int value;
public:
	Iterator(const EnumType& enum_value)
		: value{ static_cast<underlying_type>(enum_value) }
	{}

	Iterator()
		: value{ static_cast<underlying_type>(first_value) }
	{}

	Iterator operator++()
	{
		++value;
		return *this;
	}

	EnumType operator*()
	{
		return static_cast<EnumType>(value);
	}

	Iterator begin()
	{
		return *this;
	}

	Iterator end()
	{
		static const Iterator end_iterator = ++Iterator(last_value);
		return end_iterator;
	}

	bool operator!=(const Iterator& i)
	{
		return value != i.value;
	}
};