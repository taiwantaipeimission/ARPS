#include "ReportRegular.h"

ReportRegular::ReportRegular()
{
	key_list = { "A", "B", "C", "D", "NWBAP", "BAP", "CONF", "BD", "SAC", "PK", "OL", "NI", "RCLA", "LAC", "RCT" };
}


ReportRegular::~ReportRegular()
{
}

void ReportRegular::add_field(std::string key, std::string value)
{
	report_values[key] = value;
}

void ReportRegular::remove_field(std::string key)
{
	report_values.erase(key);
}

bool ReportRegular::read_unprocessed(std::istream& input)
{
	// prereq: input is after the end of a previous report, and at or before the telephone number
	// of this report.

	char str[64] = "";
	input.get(str, 64, '+');
	while (input.peek() != '8' && input.good())	//first digit of telephone number
	{
		input.getline(str, 64, '+');
	}
	input.putback('+');
	input.get(str, 14);	//13-digit telephone number
	sender_number = str;
	sender_name = "OFFICE";

	input.ignore(1000, '\n');
	input.ignore(1000, '\n');

	std::string key;
	std::string value;
	
	for (std::vector<std::string>::iterator it = key_list.begin(); it != key_list.end() && input.good(); ++it)
	{
		input.get(str, 64, ':');
		key = str;
		input.ignore(1);
		input.get(str, 64, '\n');
		value = str;
		input.ignore(1);
		report_values[*it] = value;
	}

	return (input.good());
}

void ReportRegular::read_processed(std::istream& input)
{
	input >> sender_name;
	input >> sender_number;
	std::string key_name;
	std::string value;
	input >> key_name;
	input >> value;
	if (std::count(key_list.begin(), key_list.end(), key_name) > 0)
		report_values[key_name] = value;
}

void ReportRegular::print(std::ofstream& output)
{
	output << sender_name << "\t" << sender_number << "\t";
	for (std::vector< std::string >::iterator it = key_list.begin(); it != key_list.end(); )
	{
		output << report_values[*it];
		if (++it != key_list.end())
			output << "\t";
	}
	output << std::endl;
}