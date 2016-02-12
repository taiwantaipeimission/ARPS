#include "FileManager.h"
#include <sstream>
#include <iostream>

FileManager::FileManager(std::wstring path_file_path_in)
	: path_file(path_file_path_in), config_file(), files()
{
	std::wstring config_file_path = path_file.get_field(L"CONFIG");
	if (config_file_path != L"")
	{
		config_file.filepath = config_file_path;
		config_file.open();
	}
}


FileManager::~FileManager()
{
	path_file.close();
}

bool FileManager::open_file(std::wstring file_id, File::FileType type, bool append)
{
	bool success = false;
	if (path_file.values.count(file_id) > 0)
	{
		File* file = new File(path_file.values[file_id], type, append);
		if (file->get_file()->good())
		{
			files[file_id] = file;
			success = true;
		}
	}

	if (!success)
		std::wcout << "Error opening file:" << path_file.values[file_id] << std::endl;
	return success;
}

bool FileManager::close_file(std::wstring file_id)
{
	bool success = false;
	if (files.count(file_id) > 0)
	{
		files[file_id]->close();
		delete files[file_id];
		files.erase(file_id);
		success = true;
	}
	return success;
}