#include "File.h"


File::File(std::string filepath_in, FileType file_type_in)
	: filepath(filepath_in), file_type(file_type_in)
{
	if (file_type == FILE_TYPE_INPUT)
	{
		file.open(filepath.c_str(), std::ios_base::in);
	}
	else if (file_type == FILE_TYPE_OUTPUT)
	{
		file.open(filepath.c_str(), std::ios_base::out);
	}
}

File::~File()
{
	file.close();
}

bool File::open()
{
	if (!file.is_open())
	{
		if (file_type == FILE_TYPE_INPUT)
		{
			file.open(filepath.c_str(), std::ios_base::in);
		}
		else if (file_type == FILE_TYPE_OUTPUT)
		{
			file.open(filepath.c_str(), std::ios_base::out);
		}
	}
	return file.good();
}

void File::close()
{
	if (file.is_open())
	{
		file.close();
	}
}

std::fstream* File::get_file()
{
	return &file;
}