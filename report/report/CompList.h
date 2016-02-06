#pragma once

class Area;

#include <map>
#include <string>
#include <istream>
#include <vector>

class CompList
{
public:

	static const std::string ph_number_header;
	static const std::string area_name_header;
	static const std::string zone_name_header;
	static const std::string english_unit_name_header;
	static const std::string geog_area_header;

	std::map<std::string, Area> areas;						// Keyed by phone number
	std::multimap<std::string, std::string> by_area_name;
	std::multimap<std::string, std::string> by_zone_name;
	std::multimap<std::string, std::string> by_english_unit_name;

	CompList();
	virtual ~CompList();

	void load(std::istream& input);
	void add_area(Area area);
};

