#include "Message.h"
#include "Area.h"

Message::Message()
{
}


Message::~Message()
{
}

void Message::parse(std::wstring input, CompList* comp_list)
{
	sender_number = L"";
	sender_name = L"";
	int cmgl_id_start = input.find(L"CMGL: ");
	if (cmgl_id_start != std::wstring::npos)
	{
		cmgl_id_start += 6;
		int cmgl_id_end = input.find(L",", cmgl_id_start);
		cmgl_id = input.substr(cmgl_id_start, cmgl_id_end - cmgl_id_start);
	}

	int number_start = input.find(L"+886");
	if (number_start != std::wstring::npos)
	{
		sender_number = input.substr(number_start, 13);
		if (comp_list && comp_list->areas.count(sender_number) > 0)
		{
			sender_name = comp_list->areas[sender_number].area_name;
		}
		else
			sender_name = sender_number;
	}

	//change this later!

	int type_start = input.find(L"TYPE:");
	if (type_start != std::wstring::npos)
	{
		type_start = input.find_first_not_of(' ', type_start + 5);
		int type_end = input.find(L"\n", type_start);
		std::wstring type_str = input.substr(type_start, type_end - type_start);
		if (type_str == L"REPORT")
			type = TYPE_REPORT;
		else if (type_str == L"ENGLISH")
			type = TYPE_REPORT_ENGLISH;
		else if (type_str == L"REFERRAL")
			type = TYPE_REFERRAL;
		else
			type = TYPE_UNKNOWN;
	}

	contents = input;
}