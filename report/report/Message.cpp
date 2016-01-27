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
	type = TYPE_REPORT;

	contents = input;
}