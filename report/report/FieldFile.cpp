#include "FieldFile.h"

FieldFile::FieldFile()
	: File()
{
}

FieldFile::~FieldFile()
{
}

bool FieldFile::open(FileType type_in, bool append)
{
	File::open(FILE_TYPE_INPUT);
	std::wstring line;
	std::wstring key;
	std::wstring value;

	std::getline(file, line);
	while (file.good())
	{
		int key_beg_pos = 0;
		int key_end_pos = line.find(L"=", key_beg_pos) - 1;
		int value_beg_pos = key_end_pos + 2;
		int value_end_pos = line.find(L"\n", value_beg_pos + 1) - 1;

		key = line.substr(key_beg_pos, key_end_pos - key_beg_pos + 1);
		value = line.substr(value_beg_pos, value_end_pos - value_beg_pos + 1);
		values[key] = value;

		std::getline(file, line);
	}
	return true;
}

void FieldFile::close()
{
	values.clear();
	File::close();
}

std::wstring FieldFile::get_field(std::wstring key)
{
	if (values.count(key) > 0)
	{
		return values[key];
	}
	else
	{
		return L"NA";
	}
}