#include "File.h"
#include <codecvt>
#include "utility.h"

File::File()
	: filepath(L""),
	file_type(FILE_TYPE_INPUT),
	append(false),
	file()
{

}

File::File(File& file_in)
{
	filepath = file_in.filepath;
	file_type = file_in.file_type;
	append = file_in.append;
}

File::~File()
{
	file.close();
}

bool File::open(File::FileType file_type_in)
{
	file_type = file_type_in;
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
	if (!good)
	{
		throw std::ifstream::failure(tos((L"File error in file " + filepath).c_str()));
	}
	return good;
}

void File::close()
{
	if (file.is_open())
	{
		file.close();
	}
}