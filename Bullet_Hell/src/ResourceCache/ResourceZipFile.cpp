#include "ResourceZipFile.h"

#include <climits>

#include "Logger.h"
#include "ZipFile.h"

ResourceZipFile::ResourceZipFile(const std::string resource_file_name)
	: zip_file(nullptr)
	, resource_file_name(resource_file_name)
{}

ResourceZipFile::~ResourceZipFile()
{
	zip_file = nullptr;
}

bool ResourceZipFile::open()
{
	zip_file = ZipFile::Open(resource_file_name);
	if (!zip_file)
	{
		return false;
	}
	return true;
}

int ResourceZipFile::get_raw_resource_size(const Resource& resource)
{
	if (zip_file == nullptr)
	{
		return 0;
	}
	std::shared_ptr<ZipArchiveEntry> entry = zip_file->GetEntry(resource.name);
	if (!entry)
	{
		return -1;
	}

	size_t size = entry->GetSize();
	if (size > INT_MAX)
	{
		LOG_WARNING("The resource size is larger than we can represent with an int");
		return INT_MAX;
	}
	return (int)size;
}

int ResourceZipFile::load_resource(const Resource& resource, char* buffer)
{
	
	if (zip_file == nullptr || buffer == nullptr)
	{
		return 0;
	}

	std::shared_ptr<ZipArchiveEntry> entry = zip_file->GetEntry(resource.name);
	if (!entry || !entry->CanExtract())
	{
		return 0;
	}
	size_t size = entry->GetSize();

	if (size > INT_MAX)
	{
		LOG_ERROR("We are trying to load a single file over 2 gigabytes");
		return 0;
	}

	std::istream* data_stream = entry->GetDecompressionStream();
	data_stream->read(buffer, size);

	return (int) size;
}

const int ResourceZipFile::get_resource_count()
{
	if (zip_file == nullptr)
	{
		return 0;
	}
	size_t entry_count = zip_file->GetEntriesCount();
	if (entry_count > INT_MAX)
	{
		LOG_WARNING("There are more resources than we can represent with an int");
		return INT_MAX;
	}
	return (int) entry_count;
}

std::string ResourceZipFile::get_resource_name(int index)
{
	std::string result = "";

	if (zip_file != nullptr
		&& index >= 0 && index < zip_file->GetEntriesCount())
	{
		result = zip_file->GetEntry(index)->GetFullName();
	}
	return result;
}
