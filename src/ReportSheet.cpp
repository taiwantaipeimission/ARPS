#include "ReportSheet.h"
#include "utility.h"

#include <sstream>
#include <iostream>

using namespace std;

ReportSheet::ReportSheet()
	: reports(), changed(false)
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
				//Keep incrementing until we find 1) an ID for which no record exists; 2) a an english class level name the same as this one.
				report.sub_id++;
			}
		}
		if (report_type == Report::TYPE_BAPTISM_RECORD)
		{
			report.sub_id = 0;										//The starting value for the baptism record sub-id
			while (reports.count(report.get_id_str()) > 0 && reports[report.get_id_str()].report_values[L"CONV_NAME"] != L"" && reports[report.get_id_str()].report_values[L"CONV_NAME"] != report.report_values[L"CONV_NAME"])
			{
				//Keep incrementing until we find 1) an ID for which no record exists; 2) a blank convert name (indicates unsubmitted report); 3) a convert name the same as this one.
				report.sub_id++;
			}
		}
		reports[report.get_id_str()] = report;
		changed = true;
	}
}

void ReportSheet::remove_report(std::wstring id_str)
{
	if (reports.count(id_str) > 0)
	{
		reports.erase(id_str);
		changed = true;
	}
}

void ReportSheet::read_stored_all(std::wistream& input)
{
	input.clear();
	input.seekg(0, std::ios::beg);
	if (input.good())
	{
		wchar_t line[2028] = L"";
		wstring line_s = L"";

		input.getline(line, 2028, '\n');
		header_row = line;
		vector<wstring> header_tokens = tokenize(header_row, '\t');
		
		while (input.good())
		{
			input.get(line, 2028, '\n');
			input.ignore(1, '\n');
			line_s = line;

			if (line_s.length() > 0)
			{
				Report report;
				report.set_type(report_type);
				report.read_processed(line_s, header_tokens);
				add_report(report);
			}
			else
			{
				int x = 0;
			}
		}
	}
	changed = false;
}

void ReportSheet::print(std::wostream& output)
{
	Report header_ref;
	header_ref.set_type(report_type);
	for (map<wstring, wstring>::iterator it = header_ref.report_values.begin(); it != header_ref.report_values.end(); ++it)
	{
		output << L"\t" << it->first;
	}
	output << L"\n";
	for (std::map<std::wstring, Report>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		it->second.print(output);
	}
}