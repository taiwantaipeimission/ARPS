#include "File.h"
#include "utility.h"

#include <codecvt>
#include <sstream>

File::File()
	: filepath(L""),
	file_type(FILE_TYPE_INPUT),
	append(false),
	file(),
	contents(L"")
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
	contents = L"";
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
		wcout << L"File error in file " + filepath + L"\n";
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

wstring File::extract_contents()
{
	if(contents == L"")
		contents = wstring(static_cast<wstringstream const&>(wstringstream() << file.rdbuf()).str());
	return contents;
}