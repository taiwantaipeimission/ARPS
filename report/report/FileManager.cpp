#include "FileManager.h"
#include <sstream>

FileManager::FileManager(std::string path_file_path_in)
	: path_file(path_file_path_in), config_file(), files()
{
	std::string config_file_path = path_file.get_field("CONFIG");
	if (config_file_path != "")
	{
		config_file.filepath = config_file_path;
		config_file.open();
	}
}


FileManager::~FileManager()
{
	path_file.close();
}

bool FileManager::open_file(std::string file_id, File::FileType type, bool append)
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
	return success;
}

bool FileManager::close_file(std::string file_id)
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