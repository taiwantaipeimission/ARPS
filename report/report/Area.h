#pragma once

#include <string>

class Area
{
public:
	std::string ph_number;
	std::string area_name;
	std::string geog_area;			//Name of geographical area that the proselyting area covers
	std::string zone_name;
	std::string english_unit_name;

	Area();
	virtual ~Area();
};