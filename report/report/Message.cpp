#include "Message.h"
#include "Area.h"

Message::Message()
{
}


Message::~Message()
{
}

void Message::parse(std::string input, CompList* comp_list)
{
	sender_number = "";
	sender_name = "";
	int cmgl_id_start = input.find("CMGL: ");
	if (cmgl_id_start != std::string::npos)
	{
		cmgl_id_start += 6;
		int cmgl_id_end = input.find(",", cmgl_id_start);
		cmgl_id = input.substr(cmgl_id_start, cmgl_id_end - cmgl_id_start);
	}

	int number_start = input.find("+886");
	if (number_start != std::string::npos)
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

	int type_start = input.find("TYPE:");
	if (type_start != std::string::npos)
	{
		type_start += 5;
		int type_end = input.find("\n", type_start);
		std::string type_str = input.substr(type_start, type_end - type_start);
		if (type_str == "REPORT")
			type = TYPE_REPORT;
		else if (type_str == "ENGLISH")
			type = TYPE_REPORT_ENGLISH;
		else
			type = TYPE_UNKNOWN;
	}

	contents = input;
}