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

#include "utility.h"
#include "Referral.h"
#include "Area.h"
#include <sstream>
#include <algorithm>

Referral::Referral()
{
}


Referral::~Referral()
{
}

void Referral::read_message(Message msg, std::wstring date_in)
{
	date = date_in;

	src_number = msg.get_sender_number();
	src_name = msg.get_sender_name();

	dest_geog_area = get_msg_key_val(msg.get_contents(), L"PLACE", ':', '\n');
	name = get_msg_key_val(msg.get_contents(), L"NAME", ':', '\n', false);
	number = get_msg_key_val(msg.get_contents(), L"NUMBER", ':', '\n');
	info = get_msg_key_val(msg.get_contents(), L"INFO", ':', '\n', false);

	strip_chars(name, L"\t\n");
	strip_chars(info, L"\t\n");

	//Convert dest_geog_area to uppercase
	for (std::wstring::iterator it = dest_geog_area.begin(); it != dest_geog_area.end(); ++it)
		*it = towupper(*it);
}

std::wstring Referral::print()
{
	std::wstring return_val = date + L"\t" + dest_zone + L"\t" + src_number + L"\t" + src_name + L"\t" + dest_geog_area + L"\t" + name + L"\t" + number + L"\t" + info + L"\t" + contact_state;
	return return_val;
}

void Referral::load(std::wstring line)
{
	std::vector<std::wstring> tokens = tokenize(line, '\t');

	if (tokens.size() >= 9)
	{
		int i = 0;
		date = tokens[i++];
		dest_zone = tokens[i++];
		src_number = tokens[i++];
		src_name = tokens[i++];
		dest_geog_area = tokens[i++];
		name = tokens[i++];
		number = tokens[i++];
		info = tokens[i++];
		contact_state = tokens[i++];
	}
}

bool Referral::locate(CompList* list)
{
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
		dest_number = potential_areas[choice].ph_number;
		dest_zone = potential_areas[choice].zone_name;
		return true;
	}
	else
	{
		dest_number = L"";
		return false;
	}
}

bool Referral::found_dest()
{
	return(dest_number != L"");
}