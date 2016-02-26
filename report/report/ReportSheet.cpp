#include "ReportSheet.h"
#include <sstream>
#include <iostream>
#include "utility.h"

ReportSheet::ReportSheet()
	: reports()
{
}


ReportSheet::~ReportSheet()
{
}

void ReportSheet::add_report(Report report)
{
	if (report.get_id_str() != L"")
	{
		if (report_type == Report::TYPE_ENGLISH)
		{
			report.sub_id = 0;										//The starting value for the English class level sub-id
			while (reports.count(report.get_id_str()) > 0 && reports[report.get_id_str()].report_values[L"CLASSLEVEL"] != report.report_values[L"CLASSLEVEL"])
			{
				report.sub_id++;
			}
		}
		reports[report.get_id_str()] = report;
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

		wchar_t line[2028] = L"";
		
		while (input.good())
		{
			input.get(line, 2028, '\n');
			input.ignore(1, '\n');

			id_str = line;

			if (id_str.length() > 0)
			{
				Report report;
				report.set_type(report_type);
				report.read_processed(id_str);
				reports[report.get_id_str()] = report;
			}
			else
			{
				int x = 0;
			}
		}
	}
}

void ReportSheet::print(std::wostream& output)
{
	Report header_ref;
	header_ref.set_type(report_type);
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