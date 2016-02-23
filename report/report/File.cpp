#include "File.h"
#include <codecvt>

File::File()
	: filepath(L""), file_type(FILE_TYPE_INPUT)
{
}

File::~File()
{
	file.close();
}

bool File::open(FileType type_in, bool append)
{
	file_type = type_in;
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
		file.imbue(std::locale(file.getloc(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::little_endian>()));
	}
	bool good = file.good();
	return good;
}

void File::close()
{
	if (file.is_open())
	{
		file.close();
	}
}