#include "FileManager.h"
#include <sstream>

FileManager::FileManager(std::string path_file_path_in)
	: path_file(path_file_path_in, File::FILE_TYPE_INPUT), path_file_contents(""), files()
{
	path_file_contents = static_cast<std::stringstream const&>(std::stringstream() << path_file.get_file()->rdbuf()).str();
}


FileManager::~FileManager()
{
	path_file.close();
}

bool FileManager::open_file(std::string file_id, File::FileType type)
{
	bool success = false;
	int beg_pos = path_file_contents.find("\"", path_file_contents.find(file_id));
	int end_pos = path_file_contents.find("\"", beg_pos + 1);
	if (beg_pos != std::string::npos && end_pos != std::string::npos)
	{
		std::string file_path = path_file_contents.substr(beg_pos + 1, end_pos - beg_pos - 1);
		File* file = new File(file_path, type);
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