#include "Report.h"

#include <sstream>
#include <algorithm>

#include "Message.h"

Report::Report()
{
	set_key_list();
	type = TYPE_REGULAR;
}

void Report::read_message(Message msg, std::string date)
{
	id_str = date + ":" + msg.sender_name;
	sender_number = msg.sender_number;

	std::string value;

	for (unsigned int i = 0; i < key_list.size(); i++)
	{
		int key_pos = msg.contents.find("\n" + key_list[i] + ":");
		int value_pos = msg.contents.find(':', key_pos) + 1;
		int value_end_pos = std::min(msg.contents.find('\n', value_pos), msg.contents.find('\r', value_pos));

		value = "0";
		if (key_pos != std::string::npos && value_pos != std::string::npos)
		{
			if (value_pos != value_end_pos)
			{
				value = msg.contents.substr(value_pos, value_end_pos - value_pos);
			}
		}
		report_values[key_list[i]] = value;
	}
}

Report::~Report()
{
}

void Report::set_key_list()
{
	key_list = { "A", "B", "C", "D", "NWBAP", "BAP", "CONF", "BD", "SAC", "PK", "OL", "NI", "RCLA", "LAC", "RCT" };
}

std::string Report::get_id_str()
{
	return id_str;
}

std::string Report::get_date()
{
	int last_colon_pos = id_str.find_last_of(":");
	std::string date = id_str.substr(0, last_colon_pos);
	return date;
}

std::string Report::get_sender_name()
{
	int last_colon_pos = id_str.find_last_of(":");
	std::string name = id_str.substr(last_colon_pos, id_str.length() - last_colon_pos - 2);
	return name;
}

bool Report::operator==(Report& other)
{
	if (other.id_str != this->id_str)
		return false;
	
	// sender number and cmgl_id don't matter; not retained

	if (other.key_list.size() != this->key_list.size())
		return false;

	for (int i = 0; i < key_list.size(); i++)
	{
		if (other.key_list[i] != this->key_list[i])
			return false;
		if (other.report_values[key_list[i]] != this->report_values[key_list[i]])
			return false;
	}
	return true;
}

bool Report::operator!=(Report& other)
{
	return !((*this)==other);
}

void Report::add_field(std::string key, std::string value)
{
	report_values[key] = value;
}

void Report::remove_field(std::string key)
{
	report_values.erase(key);
}

void Report::read_processed(std::string input)
{
	std::stringstream stream(input);
	stream >> id_str;
	stream >> sender_number;

	std::string key_name;
	std::string value;
	for (int i = 0; i < key_list.size(); i++)
	{
		value = "0";
		if (stream.good())
			stream >> value;
		report_values[key_list[i]] = value;
	}

	is_new = false;

	return;
}

void Report::print(std::ostream& output)
{
	output << id_str << "\t" << sender_number;
	for (int i = 0; i < key_list.size(); i++)
	{
		output << '\t' << report_values[key_list[i]];
	}
	output << std::endl;
}