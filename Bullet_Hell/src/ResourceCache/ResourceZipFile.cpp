#include "ResourceZipFile.h"

#include <climits>

#include "Logger.h"
#include "ZipFile.h"

ResourceZipFile::ResourceZipFile(const std::string resource_file_name)
	: zip_file(nullptr)
	, resource_file_name(resource_file_name)
{
	LOG_INFO("Opening the resource zip file " + resource_file_name);
}

ResourceZipFile::~ResourceZipFile()
{
	zip_file = nullptr;
}

bool ResourceZipFile::open()
{
	zip_file = ZipFile::Open(resource_file_name);
	if (!zip_file)
	{
		LOG_ERROR("Failed to open resource file " + resource_file_name);
		return false;
	}
	return true;
}

size_t ResourceZipFile::get_raw_resource_size(const Resource& resource)
{
	if (zip_file == nullptr || !zip_file)
	{
		LOG_ERROR("Resource file not valid!");
		return 0;
	}
	std::shared_ptr<ZipArchiveEntry> entry = zip_file->GetEntry(resource.name);
	if (!entry)
	{
		LOG_ERROR("Can not find the entry " + resource.name
			+ " in the resource file in order to determine size");
		return 0;
	}

	return entry->GetSize();
}

size_t ResourceZipFile::load_resource(const Resource& resource, char* buffer)
{
	
	if (zip_file == nullptr || !zip_file || buffer == nullptr)
	{
		LOG_WARNING("Resource file or destination buffer are null");
		return 0;
	}

	std::shared_ptr<ZipArchiveEntry> entry = zip_file->GetEntry(resource.name);
	if (!entry || !entry->CanExtract())
	{
		LOG_WARNING("Failed to find or extract entry " + resource.name);
		return 0;
	}
	size_t size = entry->GetSize();

	std::istream* data_stream = entry->GetDecompressionStream();
	data_stream->read(buffer, size);

	return size;
}

const size_t ResourceZipFile::get_resource_count()
{
	if (zip_file == nullptr || !zip_file)
	{
		LOG_ERROR("Resource file not valid!");
		return 0;
	}
	
	return zip_file->GetEntriesCount();
}

std::string ResourceZipFile::get_resource_name(size_t index)
{
	if (zip_file == nullptr || !zip_file)
	{
		LOG_ERROR("Resource file not valid!");
		return "";
	}

	if (index < 0 || index >= zip_file->GetEntriesCount())
	{
		LOG_ERROR("Index " + std::to_string(index) 
			+ " not valid for entries 0 through " 
			+ std::to_string(zip_file->GetEntriesCount()));
		return "";
	}
	
	return zip_file->GetEntry(static_cast<int>(index))->GetFullName();
}
