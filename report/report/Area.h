#pragma once

#include <string>

class Area
{
public:
	std::string ph_number;
	std::string area_name;
	std::string zone_name;
	std::string english_unit_name;

	Area();
	virtual ~Area();
};