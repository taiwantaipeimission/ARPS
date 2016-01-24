#include "ReportRegular.h"

#include <sstream>
#include <algorithm>

#include "Message.h"

ReportRegular::ReportRegular()
{
	key_list = { "A", "B", "C", "D", "NWBAP", "BAP", "CONF", "BD", "SAC", "PK", "OL", "NI", "RCLA", "LAC", "RCT" };
	is_new = false;
}

ReportRegular::ReportRegular(Message msg, std::string date)
{
	key_list = { "A", "B", "C", "D", "NWBAP", "BAP", "CONF", "BD", "SAC", "PK", "OL", "NI", "RCLA", "LAC", "RCT" };
	id_str = date + ":" + msg.sender_name;
	sender_number = msg.sender_number;

	std::string value;

	for (std::vector<std::string>::iterator it = key_list.begin(); it != key_list.end(); ++it)
	{
		int key_pos = msg.contents.find("\n" + *it + ":");
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
		report_values[*it] = value;
	}
	is_new = true;
}

ReportRegular::~ReportRegular()
{
}

bool ReportRegular::operator==(ReportRegular& other)
{
	if (other.id_str != this->id_str)
		return false;
	
	// sender number and cmgl_id don't matter; not retained

	if (other.key_list != this->key_list)
		return false;

	for (std::vector<std::string>::iterator it = key_list.begin(); it != key_list.end(); ++it)
	{
		if (other.report_values[*it] != this->report_values[*it])
			return false;
	}
	return true;
}

bool ReportRegular::operator!=(ReportRegular& other)
{
	return !((*this)==other);
}

void ReportRegular::add_field(std::string key, std::string value)
{
	report_values[key] = value;
}

void ReportRegular::remove_field(std::string key)
{
	report_values.erase(key);
}

void ReportRegular::read_processed(std::string input)
{
	std::stringstream stream(input);
	stream >> id_str;
	stream >> sender_number;

	std::string key_name;
	std::string value;
	for (std::vector<std::string>::iterator it = key_list.begin(); it != key_list.end(); ++it)
	{
		value = "0";
		if (stream.good())
			stream >> value;
		report_values[*it] = value;
	}

	is_new = false;

	return;
}

void ReportRegular::print(std::ostream& output)
{
	output << id_str << "\t" << sender_number;
	for (std::vector< std::string >::iterator it = key_list.begin(); it != key_list.end(); ++it)
	{
		output << '\t' << report_values[*it];
	}
	output << std::endl;
}