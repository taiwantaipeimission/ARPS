#pragma once

#include <map>
#include <string>
#include <istream>

class CompList
{
public:
	std::map<std::string, std::string> phone_name;

	CompList();
	virtual ~CompList();

	void load(std::istream& input);
};

