#pragma once
#include "File.h"

#include <map>
#include <vector>

// An input file that contains specific field values, inputted by user
class FieldFile :
	public File
{
public:
	std::map<std::wstring, std::wstring> values;

	FieldFile(std::wstring filepath_in);
	FieldFile();
	virtual ~FieldFile();

	bool FieldFile::open();
	void FieldFile::close();
	
	std::wstring get_field(std::wstring field_name);
};

