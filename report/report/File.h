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

	std::wstring filepath;
	FileType file_type;
	std::wfstream file;

	File(std::wstring filepath_in, FileType file_type_in, bool append = false);
	File();
	virtual ~File();

	bool open(bool append = false);
	void close();
	
	std::wfstream* get_file();
};

