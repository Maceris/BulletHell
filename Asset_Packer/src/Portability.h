#pragma once

#include <fstream>
#define NOMINMAX
#include <winsock2.h>

#include "glm.hpp"

/// <summary>
/// Write a value to an output stream as a 16 bit integer, in network byte 
/// order (big endian).
/// </summary>
/// <typeparam name="T">The type we are converting from.</typeparam>
/// <param name="value">The value to write.</param>
/// <param name="target">The stream to write to.</param>
template <typename T>
void constexpr write_uint16(const T& value, std::ofstream& target)
{
	static_assert(sizeof(T) <= sizeof(uint16_t),
		"Provided value does not fit in 16 bits.");
	uint16_t local = htons(std::bit_cast<uint16_t>(value));
	target.write(reinterpret_cast<const char*>(&local), sizeof(local));
}

/// <summary>
/// Write a value to an output stream as a 32 bit integer, in network byte 
/// order (big endian).
/// </summary>
/// <typeparam name="T">The type we are converting from.</typeparam>
/// <param name="value">The value to write.</param>
/// <param name="target">The stream to write to.</param>
template <typename T>
void constexpr write_uint32(const T& value, std::ofstream& target)
{
	static_assert(sizeof(T) <= sizeof(uint32_t),
		"Provided value does not fit in 32 bits.");
	uint32_t local = htonl(std::bit_cast<uint32_t>(value));
	target.write(reinterpret_cast<const char*>(&local), sizeof(local));
}

/// <summary>
/// Write a value to an output stream as a 64 bit integer, in network byte 
/// order (big endian).
/// </summary>
/// <typeparam name="T">The type we are converting from.</typeparam>
/// <param name="value">The value to write.</param>
/// <param name="target">The stream to write to.</param>
template <typename T>
void constexpr write_uint64(const T& value, std::ofstream& target)
{
	static_assert(sizeof(T) <= sizeof(uint64_t),
		"Provided value does not fit in 64 bits.");
	uint64_t local = htonll(std::bit_cast<uint64_t>(value));
	target.write(reinterpret_cast<const char*>(&local), sizeof(local));
}

/// <summary>
/// Write an array of raw bytes to file.
/// The bytes are prefixed with an integer describing the length of the byte
/// array.
/// </summary>
/// <param name="value">The values to output.</param>
/// <param name="length">The size of the array.</param>
/// <param name="target">The stream to write to.</param>
void constexpr write_data_array(const unsigned char* value, 
	const uint64_t length, std::ofstream& target)
{
	write_uint64(length, target);

	if (length > 0)
	{
		target.write(reinterpret_cast<const char*>(value), length);
	}
}

/// <summary>
/// Write a string, including prefixing the size, to the target file in network
/// byte order (big endian).
/// </summary>
/// <param name="value">The string to write.</param>
/// <param name="target">The stream to write to.</param>
void constexpr write_string(const std::string& value, std::ofstream& target)
{
	write_data_array(reinterpret_cast<const unsigned char*>(value.data()), 
		value.length(), target);
}

/// <summary>
/// Write a vec3 to the target file in network byte order (big endian).
/// Stores the output in rgba order.
/// </summary>
/// <param name="value">The vector to write.</param>
/// <param name="target">The stream to write to.</param>
void constexpr write_vec3(const glm::vec3& value, std::ofstream& target)
{
	write_uint32(value.x, target);
	write_uint32(value.y, target);
	write_uint32(value.z, target);
}

/// <summary>
/// Write a vec4 to the target file in network byte order (big endian).
/// Stores the output in rgba order.
/// </summary>
/// <param name="value">The vector to write.</param>
/// <param name="target">The stream to write to.</param>
void constexpr write_vec4(const glm::vec4& value, std::ofstream& target)
{
	write_uint32(value.r, target);
	write_uint32(value.g, target);
	write_uint32(value.b, target);
	write_uint32(value.a, target);
}

/// <summary>
/// Write a mat4 to the target file in network byte order (big endian).
/// Stores the output in column-major order.
/// </summary>
/// <param name="value">The vector to write.</param>
/// <param name="target">The stream to write to.</param>
void constexpr write_mat4(const glm::mat4& value, std::ofstream& target)
{
	for (unsigned int column = 0; column < 4; ++column)
	{
		auto& column_reference = value[column];
		for (unsigned int row = 0; row < 4; ++row)
		{
			write_uint32(column_reference[row], target);
		}
	}
}