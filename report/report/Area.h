#pragma once

#include <string>

class Area
{
public:
	std::wstring ph_number;
	std::wstring area_name;
	std::wstring geog_area;			//Name of geographical area that the proselyting area covers
	std::wstring zone_name;
	std::wstring english_unit_name;

	Area();
	virtual ~Area();
};