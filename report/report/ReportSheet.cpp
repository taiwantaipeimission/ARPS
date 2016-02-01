#include "ReportSheet.h"
#include <sstream>
#include <iostream>

ReportSheet::ReportSheet()
	: reports(), header_row("")
{
}


ReportSheet::~ReportSheet()
{
}

void ReportSheet::add_report(Report report)
{
	if (report.id_str != "")
	{
		reports[report.id_str] = report;
	}
}

void read_unprocessed(std::string input, std::string date, CompList* comp_list = NULL)
{
	int msg_start_pos = input.find("+CMGL" + 5);
	int msg_end_pos = input.find("+CMGL", msg_start_pos);

	std::string msg_string = input.substr(msg_start_pos, msg_end_pos - msg_start_pos - 1);
	// more dongxi...
}

void ReportSheet::read_stored_all(std::istream& input)
{
	input.clear();
	input.seekg(0, std::ios::beg);
	if (input.good())
	{
		getline(input, header_row);
		std::string id_str;
		std::string date;

		do
		{
			std::getline(input, id_str);
			Report report;
			report.read_processed(id_str);
			add_report(report);
		} while (input.good());
	}
}

/*void ReportSheet::read_stored_matching_date(std::istream& input, std::string date)	//date format: yyyy:m(m):w:d(d)
{
	if (input.good())
	{
		std::string id_str;
		std::string date;

		std::getline(input, id_str);
		for (; input.good(); std::getline(input, id_str))
		{
			int date_end_pos = id_str.find_last_of(":");
			date = id_str.substr(0, date_end_pos);
			std::cout << date << std::endl;
			if (date == (year + ":" + month + ":" + week + ":" + day).c_str())
			{
				Report report;
				report.read_processed(id_str);
				add_report(report);
				reports[report.sender_name].second = false;
			}
		}
	}
}*/

void ReportSheet::print(std::ostream& output)
{
	output << header_row << "\n";
	for (std::map<std::string, Report>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		it->second.print(output);
	}
}