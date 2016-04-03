/* This file is part of ARPS.
*
* (C) 2016 by David B. Elliott (elliott.david.ballantyne@gmail.com)
*
* ARPS is free software: you can redistribute it and/or modify
* it under the terms of Version 3 of the GNU General Public License as published by
* the Free Software Foundation.
*
* ARPS is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ARPS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Referral.h"
#include "Area.h"
#include "CompList.h"
#include <algorithm>

Referral::Referral()
 : Report()
{
}


Referral::~Referral()
{
}

bool Referral::locate(CompList* list)
{
	std::wstring dest_geog_area = report_values[L"PLACE"];
	strip_chars(dest_geog_area, L" ");
	std::vector<Area> potential_areas;
	for (std::map<std::wstring, Area>::iterator it = list->areas.begin(); it != list->areas.end(); ++it)
	{
		if (std::count(it->second.geog_areas.begin(), it->second.geog_areas.end(), dest_geog_area) > 0)
		{
			potential_areas.push_back(it->second);
		}
	}
	if (potential_areas.size() > 0)
	{
		size_t choice = rand() % potential_areas.size();
		report_values[L"DEST_AREA"] = potential_areas[choice].area_name;
		report_values[L"DEST_ZONE"] = potential_areas[choice].zone_name;
		report_values[L"DEST_NUMBER"] = potential_areas[choice].ph_number;
		return true;
	}
	else
	{
		report_values[L"DEST_AREA"] = L"";
		report_values[L"DEST_ZONE"] = L"";
		report_values[L"DEST_NUMBER"] = L"";
		return false;
	}
}

bool Referral::found_dest()
{
	return(report_values[L"DEST_NUMBER"] != L"");
}