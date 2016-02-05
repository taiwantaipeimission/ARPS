#pragma once

#include <map>
#include <string>
#include "File.h"
#include "FieldFile.h"

class FileManager
{
public:
	
	FieldFile path_file;
	FieldFile config_file;
	std::map<std::string, File*> files;

	FileManager(std::string path_file_path_in);
	virtual ~FileManager();

	bool open_file(std::string file_id, File::FileType type, bool append = false);
	bool close_file(std::string file_id);
};

