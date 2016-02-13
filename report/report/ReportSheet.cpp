#include "ReportSheet.h"
#include <sstream>
#include <iostream>

ReportSheet::ReportSheet()
	: reports()
{
}


ReportSheet::~ReportSheet()
{
}

void ReportSheet::add_report(Report report)
{
	if (report.id_str != L"")
	{
		reports[report.id_str] = report;
	}
}

void ReportSheet::remove_report(std::wstring id_str)
{
	if (reports.count(id_str) > 0)
	{
		reports.erase(id_str);
	}
}

void ReportSheet::read_stored_all(std::wistream& input)
{
	input.clear();
	input.seekg(0, std::ios::beg);
	if (input.good())
	{
		input.ignore(2028, '\n');	//Ignore the header row
		
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
			Report report;
			report.set_type(report_type);
			report.read_processed(id_str);
			add_report(report);
		} while (input.good());
	}
}

void ReportSheet::print(std::wostream& output)
{
	Report header_ref;
	header_ref.set_type(report_type);
	output << L"\t";
	for (int i = 0; i < header_ref.key_list.size(); i++)
	{
		output << L"\t" << header_ref.key_list[i];
	}
	output << L"\n";
	for (std::map<std::wstring, Report>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		it->second.print(output);
	}
}