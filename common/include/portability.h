#pragma once

#include "Globals.h"

#include <fstream>
#define NOMINMAX
#include <winsock2.h>

#include "glm.hpp"

#include "Debugging/Logger.h"

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