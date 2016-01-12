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

void ReportSheet::add_report(ReportRegular report)
{
	if (report.id_str != "")
	{
		reports[report.id_str] = report;
	}
}

void ReportSheet::read_unprocessed(std::istream& input, std::string date, CompList* comp_list)
{
	while (input.good())
	{
		std::string report_raw = "";

		char c;
		while (report_raw.find("+CMGL") == std::string::npos && input.good())
		{
			c = input.get();
			report_raw += c;
		}

		ReportRegular report;
		if (report.read_unprocessed(report_raw, date, comp_list))
		{
			int count = reports.count(report.id_str);
			bool equal = reports[report.id_str] == report;
			if (count == 0 || !equal)
			{
				add_report(report);
			}
		}
	}
}

void ReportSheet::read_stored_all(std::istream& input)
{
	if (input.good())
	{
		getline(input, header_row);
		std::string id_str;
		std::string date;

		do
		{
			std::getline(input, id_str);
			ReportRegular report;
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
				ReportRegular report;
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
	for (std::map<std::string, ReportRegular>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		it->second.print(output);
	}
}