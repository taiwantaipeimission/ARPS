#pragma once
#include "File.h"

#include <map>
#include <vector>

// An input file that contains specific field values, inputted by user
class FieldCollection
{
public:
	std::map<std::wstring, std::wstring> values;

	FieldCollection();
	virtual ~FieldCollection();

	void read_fields(File* file);
	
	std::wstring get_field(std::wstring field_name);
};

