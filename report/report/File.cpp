#include "File.h"


File::File(std::wstring filepath_in, FileType file_type_in, bool append)
	: filepath(filepath_in), file_type(file_type_in)
{
	open(append);
}

File::File()
	: filepath(L""), file_type(FILE_TYPE_INPUT)
{
}

File::~File()
{
	file.close();
}

bool File::open(bool append)
{
	if (!file.is_open())
	{
		std::ios_base::openmode mode = 0;
		if (file_type == FILE_TYPE_INPUT)
			mode |= std::ios_base::in;
		else if (file_type == FILE_TYPE_OUTPUT)
			mode |= std::ios_base::out;
		if (append)
			mode |= std::ios_base::app;
		file.open(filepath, mode);
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

std::wfstream* File::get_file()
{
	return &file;
}