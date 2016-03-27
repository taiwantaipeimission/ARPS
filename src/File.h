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
	bool append;
	std::wfstream file;
	std::wstring contents;

	File();
	File(File& file);
	virtual ~File();

	virtual bool open(File::FileType file_type_in);
	virtual void close();
	virtual std::wstring extract_contents();
};

