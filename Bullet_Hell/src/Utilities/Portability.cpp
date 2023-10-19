#include "Portability.h"

RawStream::RawStream()
	: data{ nullptr }
	, data_size{ 0 }
	, bytes_read{ 0 }
{}
RawStream::RawStream(unsigned char* data, const size_t size)
	: data{ data }
	, data_size{ size }
	, bytes_read{ 0 }
{}

uint8_t read_uint8(RawStream& source)
{
	uint8_t result = 0;
	if (source.bytes_read + 1 > source.data_size)
	{
		LOG_ERROR("There are "
			+ std::to_string(source.data_size - source.bytes_read)
		+" bytes left in the stream, and we want to read 1");
		return 0;
	}

	unsigned char* value = source.data + source.bytes_read;
	result = *value;
	source.bytes_read += 1;
	
	return result;
}

uint16_t read_uint16(RawStream& source)
{
	uint16_t result = 0;
	if (source.bytes_read + 2 > source.data_size)
	{
		LOG_ERROR("There are "
			+ std::to_string(source.data_size - source.bytes_read)
			+ " bytes left in the stream, and we want to read 2");
		return 0;
	}

	unsigned char* value = source.data + source.bytes_read;
	result = *(reinterpret_cast<uint16_t*>(value));
	result = ntohs(result);
	source.bytes_read += 2;

	return result;
}

uint32_t read_uint32(RawStream& source)
{
	uint32_t result = 0;
	if (source.bytes_read + 4 > source.data_size)
	{
		LOG_ERROR("There are "
			+ std::to_string(source.data_size - source.bytes_read)
			+ " bytes left in the stream, and we want to read 4");
		return 0;
	}

	unsigned char* value = source.data + source.bytes_read;
	result = *(reinterpret_cast<uint32_t*>(value));
	result = ntohl(result);
	source.bytes_read += 4;

	return result;
}

uint64_t read_uint64(RawStream& source)
{
	uint64_t result = 0;
	if (source.bytes_read + 8 > source.data_size)
	{
		LOG_ERROR("There are "
			+ std::to_string(source.data_size - source.bytes_read)
			+ " bytes left in the stream, and we want to read 8");
		return 0;
	}

	unsigned char* value = source.data + source.bytes_read;
	result = *(reinterpret_cast<uint64_t*>(value));
	result = ntohll(result);
	source.bytes_read += 8;

	return result;
}

void read_data_array(RawStream& source, unsigned char* out_result,
	uint64_t* out_length)
{
	const uint64_t size = read_uint64(source);
	*out_length = size;

	if (size == 0)
	{
		return;
	}

	if (source.bytes_read + size > source.data_size)
	{
		LOG_ERROR("There are "
			+ std::to_string(source.data_size - source.bytes_read)
			+ " bytes left in the stream, and we want to read " 
			+ std::to_string(size));
		return;
	}

	out_result = ALLOC unsigned char[size];

	if (size > 0)
	{
		memcpy(out_result, source.data, size);
		source.bytes_read += size;
	}
}

std::string read_string(RawStream& source)
{
	uint64_t size = 0;
	unsigned char* result_chars = nullptr;

	read_data_array(source, result_chars, &size);

	if (result_chars == nullptr)
	{
		return "";
	}

	std::string result(reinterpret_cast<char*>(result_chars));

	SAFE_DELETE_ARRAY(result_chars);

	return result;
}

glm::vec4 read_vec4(RawStream& source)
{
	glm::vec4 result{ 0 };
	uint32_t temp = 0;
	temp = read_uint32(source);
	result.r = *reinterpret_cast<float*>(&temp);
	temp = read_uint32(source);
	result.g = *reinterpret_cast<float*>(&temp);
	temp = read_uint32(source);
	result.b = *reinterpret_cast<float*>(&temp);
	temp = read_uint32(source);
	result.a = *reinterpret_cast<float*>(&temp);
	return result;
}

glm::mat4 read_mat4(RawStream& source)
{
	glm::mat4 result{ 0 };

	uint32_t temp = 0;
	for (unsigned int column = 0; column < 4; ++column)
	{
		for (unsigned int row = 0; row < 4; ++row)
		{
			temp = read_uint32(source);
			result[column][row] = *reinterpret_cast<float*>(&temp);
		}
	}
	return result;
}
