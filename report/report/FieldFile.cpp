#include "FieldFile.h"


FieldFile::FieldFile(std::string filepath_in)
: File(filepath_in, FILE_TYPE_INPUT)
{
	open();
}

FieldFile::FieldFile()
	: File()
{
}

FieldFile::~FieldFile()
{
}

bool FieldFile::open()
{
	File::open();
	std::string line;
	std::string key;
	std::string value;

	std::getline(file, line);
	while (file.good())
	{
		int key_beg_pos = 0;
		int key_end_pos = line.find("=", key_beg_pos) - 1;
		int value_beg_pos = key_end_pos + 2;
		int value_end_pos = line.find("\n", value_beg_pos + 1) - 1;

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

std::string FieldFile::get_field(std::string key)
{
	if (values.count(key) > 0)
	{
		return values[key];
	}
	else
	{
		return "NA";
	}
}