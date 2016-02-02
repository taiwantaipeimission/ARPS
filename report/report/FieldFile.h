#pragma once
#include "File.h"

#include <map>
#include <vector>

// An input file that contains specific field values, inputted by user
class FieldFile :
	public File
{
public:
	std::map<std::string, std::string> values;

	FieldFile(std::string filepath_in);
	FieldFile();
	virtual ~FieldFile();

	bool FieldFile::open();
	void FieldFile::close();
	
	std::string get_field(std::string field_name);
};

