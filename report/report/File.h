#pragma once

#include <string>
#include <fstream>

class File
{
public:
	enum FileType
	{
		FILE_TYPE_INPUT,
		FILE_TYPE_OUTPUT
	};

	std::string filepath;
	FileType file_type;
	std::fstream file;

	File(std::string filepath_in, FileType file_type_in, bool append = false);
	File();
	virtual ~File();

	bool open(bool append = false);
	void close();
	
	std::fstream* get_file();
};

