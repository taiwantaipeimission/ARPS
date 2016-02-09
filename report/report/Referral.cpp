#include "Referral.h"
#include "Area.h"
#include <sstream>

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

	std::wstringstream ss(msg.contents);
	ss.ignore(1000, '\n');	//get rid of type line
	std::getline(ss, dest_geog_area);
	std::getline(ss, name);
	std::getline(ss, info);
}

bool Referral::locate(CompList* list)
{
	std::vector<std::wstring> potential_numbers;
	for (std::map<std::wstring, Area>::iterator it = list->areas.begin(); it != list->areas.end(); ++it)
	{
		if (it->second.geog_area == dest_geog_area)
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