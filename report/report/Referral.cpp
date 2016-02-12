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

std::wstring find_value(std::wstring msg_contents, std::wstring key)
{
	int key_pos = msg_contents.find(L"\n" + key + L":");
	int value_pos = msg_contents.find(':', key_pos) + 1;
	int value_end_pos = std::min(msg_contents.find('\n', value_pos), msg_contents.find('\r', value_pos));

	std::wstring value = L"0";
	if (key_pos != std::wstring::npos && value_pos != std::wstring::npos)
	{
		if (value_pos != value_end_pos)
		{
			value = msg_contents.substr(value_pos, value_end_pos - value_pos);
			value.erase(std::remove(value.begin(), value.end(), ' '), value.end());		//Strip whitespace from string
			value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());
		}
	}
	return value;
}

void Referral::read_message(Message msg)
{
	src_number = msg.sender_number;
	src_name = msg.sender_name;

	dest_geog_area = find_value(msg.contents, L"PLACE");
	name = find_value(msg.contents, L"NAME");
	number = find_value(msg.contents, L"NUMBER");
	info = find_value(msg.contents, L"INFO");
}

bool Referral::locate(CompList* list)
{
	std::vector<std::wstring> potential_numbers;
	for (std::map<std::wstring, Area>::iterator it = list->areas.begin(); it != list->areas.end(); ++it)
	{
		if (std::count(it->second.geog_areas.begin(), it->second.geog_areas.end(), dest_geog_area) > 0)
		{
			potential_numbers.push_back(it->first);
		}
	}
	if (potential_numbers.size() > 0)
	{
		size_t choice = rand() % potential_numbers.size();
		dest_number = potential_numbers[choice];
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