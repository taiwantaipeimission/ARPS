#include "CompList.h"

#include <iostream>
#include <sstream>

#include "Area.h"

const std::string CompList::ph_number_header = "PH_NUMBER";
const std::string CompList::area_name_header = "AREA_NAME";
const std::string CompList::zone_name_header = "ZONE_NAME";
const std::string CompList::english_unit_name_header = "ENGLISH_UNIT_NAME";
const std::string CompList::geog_area_header = "GEOG_AREAS";

CompList::CompList()
{
}


CompList::~CompList()
{
}

void CompList::load(std::istream& input)
{
	std::string line;
	std::getline(input, line);

	std::stringstream strstr(line);

	// use stream iterators to copy the stream to the vector as whitespace separated strings
	std::istream_iterator<std::string> it(strstr);
	std::istream_iterator<std::string> end;
	std::vector<std::string> header(it, end);

	while (input.good())
	{
		std::getline(input, line);
		std::stringstream linestr(line);
		std::istream_iterator<std::string> line_it(linestr);
		std::vector<std::string> results(line_it, end);
		std::vector<std::string>::iterator results_i = results.begin();
		std::vector<std::string>::iterator header_i = header.begin();

		Area area_to_add;
		for (header_i = header.begin(), results_i = results.begin(); header_i != header.end() && results_i != results.end(); ++header_i, ++results_i)
		{
			if (*header_i == ph_number_header)
			{
				area_to_add.ph_number = *results_i;
			}
			if (*header_i == area_name_header)
			{
				area_to_add.area_name = *results_i;
			}
			if (*header_i == zone_name_header)
			{
				area_to_add.zone_name = *results_i;
			}
			if (*header_i == english_unit_name_header)
			{
				area_to_add.english_unit_name = *results_i;
			}
			if (*header_i == geog_area_header)
			{
				area_to_add.geog_area = *results_i;
			}
		}
		
		if (area_to_add.area_name != "")
		{
			add_area(area_to_add);
		}
	}
}

void CompList::add_area(Area area)
{
	areas.insert(std::pair<std::string, Area>(area.ph_number, area));
	by_area_name.insert(std::pair<std::string, std::string>(area.area_name, area.ph_number));
	by_zone_name.insert(std::pair<std::string, std::string>(area.zone_name, area.ph_number));
	by_english_unit_name.insert(std::pair<std::string, std::string>(area.english_unit_name, area.ph_number));
}