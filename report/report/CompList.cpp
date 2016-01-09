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
	input >> ph_number >> name;
	while (input.good())
	{
		phone_name[ph_number] = name;
		input >> ph_number >> name;
	}
}