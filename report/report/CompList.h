#pragma once

class Area;

#include <map>
#include <string>
#include <istream>
#include <vector>

class CompList
{
public:

	static const std::wstring ph_number_header;
	static const std::wstring area_name_header;
	static const std::wstring zone_name_header;
	static const std::wstring english_unit_name_header;
	static const std::wstring geog_area_header;

	std::map<std::wstring, Area> areas;						// Keyed by phone number
	std::multimap<std::wstring, std::wstring> by_area_name;
	std::multimap<std::wstring, std::wstring> by_zone_name;
	std::multimap<std::wstring, std::wstring> by_english_unit_name;

	CompList();
	virtual ~CompList();

	void load(std::wistream& input);
	void add_area(Area area);
};

