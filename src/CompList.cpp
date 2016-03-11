#include "codes.h"
#include "CompList.h"
#include "utility.h"

#include <iostream>
#include <sstream>
#include <iterator>

#include "Area.h"

const std::wstring CompList::ph_number_header = L"PH_NUMBER";
const std::wstring CompList::area_name_header = L"AREA_NAME";
const std::wstring CompList::district_name_header = L"DISTRICT_NAME";
const std::wstring CompList::zone_name_header = L"ZONE_NAME";
const std::wstring CompList::ward_name_header = L"WARD_NAME";
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

void CompList::load(FileManager* file_manager)
{
	file_manager->files[FILE_PH_LIST].open(File::FILE_TYPE_INPUT);
	std::wstring line;
	std::getline(file_manager->files[FILE_PH_LIST].file, line);
	std::vector<std::wstring> header = tokenize(line, '\t');

	while (file_manager->files[FILE_PH_LIST].file.good())
	{
		std::getline(file_manager->files[FILE_PH_LIST].file, line);
		std::vector<std::wstring> results = tokenize(line, '\t');
		std::vector<std::wstring>::iterator results_i = results.begin();
		std::vector<std::wstring>::iterator header_i = header.begin();

		Area area_to_add;
		for (header_i = header.begin(), results_i = results.begin(); header_i != header.end() && results_i != results.end(); ++header_i, ++results_i)
		{
			if (*header_i == ph_number_header)
			{
				if (results_i->length() > 0)
					area_to_add.ph_number = INT_PH_NUM_PREFIX + results_i->substr(1, results_i->length() - 1);
				else
					area_to_add.ph_number = L"";
			}
			else if (*header_i == area_name_header)
			{
				area_to_add.area_name = *results_i;
			}
			else if (*header_i == district_name_header)
			{
				area_to_add.district_name = *results_i;
			}
			else if (*header_i == ward_name_header)
			{
				area_to_add.ward_name = *results_i;
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
	file_manager->files[FILE_PH_LIST].close();
}

void CompList::add_area(Area area)
{
	areas.insert(std::pair<std::wstring, Area>(area.ph_number, area));
	by_area_name[area.area_name].push_back(area);
	by_zone_name[area.zone_name].push_back(area);
	by_english_unit_name[area.english_unit_name].push_back(area);
}