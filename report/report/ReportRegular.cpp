#include "ReportRegular.h"

#include <sstream>
#include <algorithm>

ReportRegular::ReportRegular()
{
	key_list = { "A", "B", "C", "D", "NWBAP", "BAP", "CONF", "BD", "SAC", "PK", "OL", "NI", "RCLA", "LAC", "RCT" };
}


ReportRegular::~ReportRegular()
{
}

bool ReportRegular::operator==(ReportRegular& other)
{
	if (other.id_str != this->id_str)
		return false;
	
	// sender number doesn't matter; not retained

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

bool ReportRegular::read_unprocessed(std::string input, std::string date, CompList* comp_list)
{
	std::string sender_number = "";
	std::string sender_name = "";
	int number_start = input.find("+886");
	if (number_start != std::string::npos)
	{
		sender_number = input.substr(number_start, 13);
		if (comp_list && comp_list->phone_name.count(sender_number) > 0)
			sender_name = comp_list->phone_name[sender_number];
		else
			sender_name = sender_number;

		id_str = date + ":" + sender_name;

		std::string value;

		for (std::vector<std::string>::iterator it = key_list.begin(); it != key_list.end(); ++it)
		{
			int key_pos = input.find(*it + ":");
			int value_pos = input.find(':', key_pos) + 1;
			int value_end_pos = std::min(input.find('\n', value_pos), input.find('\r', value_pos));

			value = "0";
			if (key_pos != std::string::npos && value_pos != std::string::npos)
			{
				if (value_pos != value_end_pos)
				{
					value = input.substr(value_pos, value_end_pos - value_pos);
				}
			}
			report_values[*it] = value;
		}
		return true;
	}
	else
	{
		return false;	//reading failed;
	}
}

void ReportRegular::read_processed(std::string input)
{
	std::stringstream stream(input);
	stream >> id_str;

	std::string key_name;
	std::string value;
	for (std::vector<std::string>::iterator it = key_list.begin(); it != key_list.end(); ++it)
	{
		value = "0";
		if (stream.good())
			stream >> value;
		report_values[*it] = value;
	}

	return;
}

void ReportRegular::print(std::ostream& output)
{
	output << id_str;
	for (std::vector< std::string >::iterator it = key_list.begin(); it != key_list.end(); ++it)
	{
		output << '\t' << report_values[*it];
	}
	output << std::endl;
}