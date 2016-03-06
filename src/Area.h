#pragma once

#include <string>
#include <vector>

class Area
{
public:
	std::wstring ph_number;
	std::wstring area_name;
	std::vector<std::wstring> geog_areas;			//Names of geographical areas that the proselyting area covers
	
	std::wstring district_name;
	std::wstring zone_name;
	std::wstring ward_name;
	std::wstring stake_name;
	std::wstring english_unit_name;
	bool report_required;
	bool english_required;

	Area();
	virtual ~Area();
};