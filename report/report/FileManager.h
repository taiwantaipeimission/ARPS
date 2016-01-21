#pragma once

#include <map>
#include <string>
#include "File.h"

class FileManager
{
public:
	
	File path_file;
	std::string path_file_contents;
	std::map<std::string, File*> files;

	FileManager(std::string path_file_path_in);
	virtual ~FileManager();

	bool open_file(std::string file_id, File::FileType type);
	bool close_file(std::string file_id);
};

