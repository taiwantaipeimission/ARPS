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

#pragma once

#include "Area.h"

#include <map>
#include <string>
#include <istream>
#include <vector>
#include "File.h"
#include "FileManager.h"

class CompList
{
public:

	static const std::wstring ph_number_header;
	static const std::wstring area_name_header;
	static const std::wstring district_name_header;
	static const std::wstring zone_name_header;
	static const std::wstring ward_name_header;
	static const std::wstring stake_name_header; 
	static const std::wstring english_unit_name_header;
	static const std::wstring geog_area_header;
	static const std::wstring report_required_header;
	static const std::wstring english_required_header;
	
	std::map<std::wstring, Area> areas;						// Keyed by phone number
	std::map<std::wstring, std::vector<Area>> by_area_name;
	std::map<std::wstring, std::vector<Area>> by_zone_name;
	std::map<std::wstring, std::vector<Area>> by_english_unit_name;

	CompList();
	virtual ~CompList();

	void load(FileManager* file_manager);
	void add_area(Area area);
};

