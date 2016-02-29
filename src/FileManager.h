#pragma once

#include <map>
#include "File.h"
#include "FieldCollection.h"

class FileManager
{
public:

	FieldCollection paths;
	std::map<std::wstring, File> files;			//Files, organized by ID

	FileManager();
	virtual ~FileManager();

	/* Populate list of files with every file found in the paths file, including the proper path for each */
	void load(std::wstring path_file_path);
};

