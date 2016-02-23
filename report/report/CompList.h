#pragma once

class Area;

#include <map>
#include <string>
#include <istream>
#include <vector>
#include "File.h"

class CompList
{
public:

	static const std::wstring ph_number_header;
	static const std::wstring area_name_header;
	static const std::wstring district_name_header;
	static const std::wstring zone_name_header;
	static const std::wstring stake_name_header; 
	static const std::wstring english_unit_name_header;
	static const std::wstring geog_area_header;
	static const std::wstring report_required_header;
	static const std::wstring english_required_header;
	
	File comp_list_file;
	std::map<std::wstring, Area> areas;						// Keyed by phone number
	std::multimap<std::wstring, std::wstring> by_area_name;
	std::multimap<std::wstring, std::wstring> by_zone_name;
	std::multimap<std::wstring, std::wstring> by_english_unit_name;

	CompList();
	virtual ~CompList();

	void load();
	void add_area(Area area);
};

