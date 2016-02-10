#include "ReportSheet.h"
#include <sstream>
#include <iostream>

#include "ReportEnglish.h"

ReportSheet::ReportSheet()
	: report_type(Report::TYPE_REGULAR), reports(), header_row(L"")
{
}


ReportSheet::~ReportSheet()
{
	for (std::map<std::wstring, Report*>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		delete it->second;
	}
}

void ReportSheet::add_report(Report* report)
{
	if (report->id_str != L"")
	{
		reports[report->id_str] = report;
	}
}

void ReportSheet::remove_report(std::wstring id_str)
{
	if (reports.count(id_str) > 0)
	{
		delete reports[id_str];
		reports.erase(id_str);
	}
}

void ReportSheet::read_stored_all(std::wistream& input)
{
	input.clear();
	input.seekg(0, std::ios::beg);
	if (input.good())
	{
		getline(input, header_row);
		
		std::wstring id_str;
		std::wstring date;

		do
		{
			wchar_t line[2028] = L"";
			input.get(line, 2028, '\n');
			input.ignore(1, '\n');
			id_str = line;
			bool eof = input.eof();
			bool good = input.good();
			bool fail = input.fail();
			bool bad = input.bad();
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

void ReportSheet::print(std::wostream& output)
{
	output << header_row << L"\n";
	for (std::map<std::wstring, Report*>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		it->second->print(output);
	}
}