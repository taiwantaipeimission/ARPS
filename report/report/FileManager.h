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
	std::map<std::wstring, File*> files;

	FileManager(std::wstring path_file_path_in);
	virtual ~FileManager();

	bool open_file(std::wstring file_id, File::FileType type, bool append = false);
	bool close_file(std::wstring file_id);
};

