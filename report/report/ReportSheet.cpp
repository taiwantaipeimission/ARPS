#include "ReportSheet.h"
#include <sstream>
#include <iostream>

#include "ReportEnglish.h"

ReportSheet::ReportSheet()
	: report_type(Report::TYPE_REGULAR), reports(), header_row("")
{
}


ReportSheet::~ReportSheet()
{
	for (std::map<std::string, Report*>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		delete it->second;
	}
}

void ReportSheet::add_report(Report* report)
{
	if (report->id_str != "")
	{
		reports[report->id_str] = report;
	}
}

void ReportSheet::remove_report(std::string id_str)
{
	if (reports.count(id_str) > 0)
	{
		delete reports[id_str];
		reports.erase(id_str);
	}
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
			Report* report;
			if (report_type == Report::TYPE_REGULAR)
			{
				report = new Report();
				report->read_processed(id_str);
				add_report(report);
			}
			else if (report_type == Report::TYPE_ENGLISH)
			{
				report = new ReportEnglish();
				report->read_processed(id_str);
				add_report(report);
			}

			
		} while (input.good());
	}
}

void ReportSheet::print(std::ostream& output)
{
	output << header_row << "\n";
	for (std::map<std::string, Report*>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		it->second->print(output);
	}
}