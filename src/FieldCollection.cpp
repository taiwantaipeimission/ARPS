#include "FieldCollection.h"

FieldCollection::FieldCollection()
 : values()
{

}

FieldCollection::~FieldCollection()
{

}

void FieldCollection::read_fields(File* file)
{
	std::wstring line;
	std::wstring key;
	std::wstring value;

	std::getline(file->file, line);
	while (file->file.good())
	{
		int key_beg_pos = 0;
		int key_end_pos = line.find(L"=", key_beg_pos) - 1;
		int value_beg_pos = key_end_pos + 2;
		int value_end_pos = line.find(L"\n", value_beg_pos + 1) - 1;

		key = line.substr(key_beg_pos, key_end_pos - key_beg_pos + 1);
		value = line.substr(value_beg_pos, value_end_pos - value_beg_pos + 1);
		values[key] = value;

		std::getline(file->file, line);
	}
}

std::wstring FieldCollection::get_field(std::wstring key)
{
	if (values.count(key) > 0)
	{
		return values[key];
	}
	else
	{
		return L"-";
	}
}