#include "CompList.h"

#include <iostream>
#include <sstream>

#include "Area.h"

const std::wstring CompList::ph_number_header = L"PH_NUMBER";
const std::wstring CompList::area_name_header = L"AREA_NAME";
const std::wstring CompList::district_name_header = L"DISTRICT_NAME";
const std::wstring CompList::zone_name_header = L"ZONE_NAME";
const std::wstring CompList::stake_name_header = L"STAKE_NAME";
const std::wstring CompList::english_unit_name_header = L"ENGLISH_UNIT_NAME";
const std::wstring CompList::geog_area_header = L"GEOG_AREAS";
const std::wstring CompList::report_required_header = L"REPORT_REQUIRED";
const std::wstring CompList::english_required_header = L"ENGLISH_REQUIRED";

CompList::CompList()
{
}


CompList::~CompList()
{
}

void CompList::load(std::wistream& input)
{
	std::wstring line;
	std::getline(input, line);

	std::wstringstream strstr(line);

	// use stream iterators to copy the stream to the vector as whitespace separated strings
	std::istream_iterator<std::wstring, wchar_t> it(strstr);
	std::istream_iterator<std::wstring, wchar_t> end;
	std::vector<std::wstring> header(it, end);

	while (input.good())
	{
		std::getline(input, line);
		std::wstringstream linestr(line);
		std::istream_iterator<std::wstring, wchar_t> line_it(linestr);
		std::vector<std::wstring> results(line_it, end);
		std::vector<std::wstring>::iterator results_i = results.begin();
		std::vector<std::wstring>::iterator header_i = header.begin();

		Area area_to_add;
		for (header_i = header.begin(), results_i = results.begin(); header_i != header.end() && results_i != results.end(); ++header_i, ++results_i)
		{
			if (*header_i == ph_number_header)
			{
				area_to_add.ph_number = *results_i;
			}
			else if (*header_i == area_name_header)
			{
				area_to_add.area_name = *results_i;
				if (area_to_add.area_name == L"435")
				{
					int x = 1;
				}
			}
			else if (*header_i == district_name_header)
			{
				area_to_add.district_name = *results_i;
			}
			else if (*header_i == zone_name_header)
			{
				area_to_add.zone_name = *results_i;
			}
			else if (*header_i == stake_name_header)
			{
				area_to_add.stake_name = *results_i;
			}
			else if (*header_i == english_unit_name_header)
			{
				area_to_add.english_unit_name = *results_i;
			}
			else if (*header_i == report_required_header)
			{
				area_to_add.report_required = (*results_i == L"1" ? true : false);
			}
			else if (*header_i == english_required_header)
			{
				area_to_add.english_required = (*results_i == L"1" ? true : false);
			}
			else if (*header_i == geog_area_header)
			{
				std::wstringstream ss(*results_i);
				std::wstring geog_area_name = L"";
				while (std::getline(ss, geog_area_name, L','))
				{
					area_to_add.geog_areas.push_back(geog_area_name);
				}
			}
		}
		
		if (area_to_add.area_name != L"")
		{
			add_area(area_to_add);
		}
	}
}

void CompList::add_area(Area area)
{
	areas.insert(std::pair<std::wstring, Area>(area.ph_number, area));
	by_area_name.insert(std::pair<std::wstring, std::wstring>(area.area_name, area.ph_number));
	by_zone_name.insert(std::pair<std::wstring, std::wstring>(area.zone_name, area.ph_number));
	by_english_unit_name.insert(std::pair<std::wstring, std::wstring>(area.english_unit_name, area.ph_number));
}