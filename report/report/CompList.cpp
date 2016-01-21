#include "CompList.h"
#include <iostream>

CompList::CompList()
{
}


CompList::~CompList()
{
}

void CompList::load(std::istream& input)
{
	std::string ph_number;
	std::string name;
	std::string zone;
	input >> ph_number >> name >> zone;
	while (input.good())
	{
		phone_name[ph_number].first = name;
		phone_name[ph_number].second = zone;
		input >> ph_number >> name >> zone;
	}
}