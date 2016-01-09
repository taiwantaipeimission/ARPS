#include "ReportSheet.h"
#include <sstream>
#include <iostream>

ReportSheet::ReportSheet(std::string year_in, std::string month_in, std::string week_in, std::string day_in)
	: year(year_in), month(month_in), week(week_in), day(day_in), reports()
{
}


ReportSheet::~ReportSheet()
{
}

void ReportSheet::add_report(ReportRegular report)
{
	if (report.sender_name != "")
	{
		reports[report.sender_name] = report;
	}
}

void ReportSheet::read_unprocessed(std::istream& input)
{
	while (input.good())
	{
		ReportRegular report;
		if (report.read_unprocessed(input))
		{
			add_report(report);
		}
	}
}

void ReportSheet::read_processed(std::istream& input)
{
	std::string date;

	input >> date;
	for (; input.good(); input >> date)
	{
		std::cout << date << std::endl;
		if (date == (year + ":" + month + ":" + week + ":" + day).c_str())
		{
			ReportRegular report;
			report.read_processed(input);
			add_report(report);
		}
		input.ignore(1000, '\n');
	}
}

void ReportSheet::print(std::ofstream& output)
{
	std::cout << "Printing" << std::endl;
	for (std::map<std::string, ReportRegular>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		std::cout << "Printing:" << it->second.sender_number << std::endl;
		output << year << ":" << month << ":" << week << ":" << day << "\t";
		it->second.print(output);
	}
}