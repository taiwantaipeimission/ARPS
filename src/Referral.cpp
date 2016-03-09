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

void Referral::read_message(Message msg)
{
	src_number = msg.sender_number;
	src_name = msg.sender_name;

	dest_geog_area = get_msg_key_val(msg.contents, L"PLACE", ':', '\n');
	name = get_msg_key_val(msg.contents, L"NAME", ':', '\n');
	number = get_msg_key_val(msg.contents, L"NUMBER", ':', '\n');
	info = get_msg_key_val(msg.contents, L"INFO", ':', '\n');

	//Convert dest_geog_area to uppercase
	for (std::wstring::iterator it = dest_geog_area.begin(); it != dest_geog_area.end(); ++it)
		*it = towupper(*it);
}

std::wstring Referral::print(std::wstring date)
{
	std::wstring return_val = date + L"\t" + dest_zone + L"\t" + src_number + L"\t" + src_name + L"\t" + dest_geog_area + L"\t" + name + L"\t" + number + L"\t" + info;
	return return_val;
}

void Referral::load(std::wstring line)
{
	std::wstring date;

	std::vector<std::wstring> tokens = tokenize(line, '\t');

	if (tokens.size() >= 8)
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