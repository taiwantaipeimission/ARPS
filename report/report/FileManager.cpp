#include "FileManager.h"
#include <sstream>

FileManager::FileManager(std::string path_file_path_in)
	: path_file(path_file_path_in, File::FILE_TYPE_INPUT), files(), paths()
{
	std::string line;
	std::string file_id;
	std::string file_path;
	std::getline(*path_file.get_file(), line);

	while (path_file.get_file()->good())
	{
		int id_beg_pos = 0;
		int id_end_pos = line.find("=", id_beg_pos);
		int path_beg_pos = line.find("\"", id_end_pos + 1);
		int path_end_pos = line.find("\"", path_beg_pos + 1);
		
		file_id = line.substr(id_beg_pos, id_end_pos - id_beg_pos);
		file_path = line.substr(path_beg_pos + 1, path_end_pos - path_beg_pos - 1);
		paths[file_id] = file_path;

		std::getline(*path_file.get_file(), line);
	}
}


FileManager::~FileManager()
{
	path_file.close();
}

bool FileManager::open_file(std::string file_id, File::FileType type)
{
	bool success = false;
	if (paths.count(file_id) > 0)
	{
		File* file = new File(paths[file_id], type);
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