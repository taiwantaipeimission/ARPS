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

	File();
	virtual ~File();

	virtual bool open(FileType type, bool append = false);
	virtual void close();
};

