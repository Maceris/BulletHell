#pragma once

#include <bit>
#include <fstream>
#include <winsock2.h>

#include "glm/glm.hpp"

/// <summary>
/// Represents using a raw char stream as if it was an io stream.
/// </summary>
struct RawStream
{
	/// <summary>
	/// A reference to the raw data we are reading through.
	/// </summary>
	unsigned char* data;

	/// <summary>
	/// The total number of bytes in the data.
	/// </summary>
	size_t data_size;

	/// <summary>
	/// The number of bytes we have read so far.
	/// </summary>
	size_t bytes_read;

	RawStream();
	RawStream(unsigned char* data, const size_t size);
	~RawStream() = default;
};

/// <summary>
/// Load a byte from the stream.
/// </summary>
/// <param name="source">The source to read from.</param>
/// <returns>The result in host byte order.</returns>
uint8_t read_uint8(RawStream& source);

/// <summary>
/// Load an unsigned 16 bit integer from the stream.
/// </summary>
/// <param name="source">The source to read from.</param>
/// <returns>The result in host byte order.</returns>
uint16_t read_uint16(RawStream& source);

/// <summary>
/// Load an unsigned 32 bit integer from the stream.
/// </summary>
/// <param name="source">The source to read from.</param>
/// <returns>The result in host byte order.</returns>
uint32_t read_uint32(RawStream& source);

/// <summary>
/// Load an unsigned 64 bit integer from the stream.
/// </summary>
/// <param name="source">The source to read from.</param>
/// <returns>The result in host byte order.</returns>
uint64_t read_uint64(RawStream& source);

/// <summary>
/// Reads an array of data as raw bytes.
/// </summary>
/// <param name="source">The source to read from.</param>
/// <param name="out_result">A pointer that stores the result, you should 
/// pass in a pointer to nullptr as this will allocate memory for you.</param>
/// <param name="out_length">Where to store the length of the returned array.
/// </param>
/// <param name="null_terminate">Whether we should include a null byte
/// at the end of the returned array. Useful for strings, but not always
/// desireable.
/// </param>
void read_data_array(RawStream& source, unsigned char** out_result,
	uint64_t* out_length, const bool null_terminate = false);

/// <summary>
/// Load a string from the stream.
/// </summary>
/// <param name="source">The source to read from.</param>
/// <returns>The string we have read.</returns>
std::string read_string(RawStream& source);

/// <summary>
/// Load a vec3 from the stream.
/// </summary>
/// <param name="source">The source to read from.</param>
/// <returns>The vec3 we have read, in host byte order.</returns>
glm::vec3 read_vec3(RawStream& source);

/// <summary>
/// Load a vec4 from the stream.
/// </summary>
/// <param name="source">The source to read from.</param>
/// <returns>The vec4 we have read, in host byte order.</returns>
glm::vec4 read_vec4(RawStream& source);

/// <summary>
/// Load a mat4 from the stream.
/// </summary>
/// <param name="source">The source to read from.</param>
/// <returns>The mat4 we have read, in host byte order.</returns>
glm::mat4 read_mat4(RawStream& source);

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
void write_data_array(const unsigned char* value, const uint64_t length,
	std::ofstream& target);

/// <summary>
/// Write a string, including prefixing the size, to the target file in network
/// byte order (big endian).
/// </summary>
/// <param name="value">The string to write.</param>
/// <param name="target">The stream to write to.</param>
void write_string(const std::string& value, std::ofstream& target);

/// <summary>
/// Write a vec3 to the target file in network byte order (big endian).
/// Stores the output in rgba order.
/// </summary>
/// <param name="value">The vector to write.</param>
/// <param name="target">The stream to write to.</param>
void write_vec3(const glm::vec3& value, std::ofstream& target);

/// <summary>
/// Write a vec4 to the target file in network byte order (big endian).
/// Stores the output in rgba order.
/// </summary>
/// <param name="value">The vector to write.</param>
/// <param name="target">The stream to write to.</param>
void write_vec4(const glm::vec4& value, std::ofstream& target);

/// <summary>
/// Write a mat4 to the target file in network byte order (big endian).
/// Stores the output in column-major order.
/// </summary>
/// <param name="value">The vector to write.</param>
/// <param name="target">The stream to write to.</param>
void write_mat4(const glm::mat4& value, std::ofstream& target);
