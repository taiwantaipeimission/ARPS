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