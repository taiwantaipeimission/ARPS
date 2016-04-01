/* This file is part of ARPS.
*
* (C) 2016 by David B. Elliott (elliott.david.ballantyne@gmail.com)
*
* ARPS is free software: you can redistribute it and/or modify
* it under the terms of Version 3 of the GNU General Public License as published by
* the Free Software Foundation.
*
* ARPS is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with ARPS.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ReportSheet.h"
#include "utility.h"

#include <sstream>
#include <iostream>

using namespace std;

ReportSheet::ReportSheet()
	: reports(), changed(false), loaded(false)
{
}


ReportSheet::~ReportSheet()
{
}

void ReportSheet::add_report(Report report)
{
	if (report.get_id_str() != L"")
	{
		if (report_type == TYPE_ENGLISH)
		{
			report.sub_id = 0;										//The starting value for the English class level sub-id
			while (reports.count(report.get_id_str()) > 0 && reports[report.get_id_str()].report_values[L"CLASSLEVEL"] != report.report_values[L"CLASSLEVEL"])
			{
				//Keep incrementing until we find 1) an ID for which no record exists; 2) a an english class level name the same as this one.
				report.sub_id++;
			}
		}
		if (report_type == TYPE_BAPTISM_RECORD)
		{
			report.sub_id = 0;										//The starting value for the baptism record sub-id
			while (reports.count(report.get_id_str()) > 0 && reports[report.get_id_str()].report_values[L"CONV_NAME"] != L"" && reports[report.get_id_str()].report_values[L"CONV_NAME"] != report.report_values[L"CONV_NAME"])
			{
				//Keep incrementing until we find 1) an ID for which no record exists; 2) a blank convert name (indicates unsubmitted report); 3) a convert name the same as this one.
				report.sub_id++;
			}
		}
		if (report_type == TYPE_REFERRAL)
		{
			report.sub_id = 0;
			while (reports.count(report.get_id_str()) > 0)
			{
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
		sheet_fields = tokenize(line, L'\t');
		sheet_fields.erase(sheet_fields.begin());	//Get rid of the first blank column (the ID string column)
		
		while (input.good())
		{
			input.get(line, 2028, '\n');
			input.ignore(1, '\n');
			line_s = line;

			if (line_s.length() > 0)
			{
				Report report;
				report.set_type(report_type);
				report.read_processed(line_s, sheet_fields);
				add_report(report);
			}
			else
			{
				int x = 0;
			}
		}
	}
	changed = false;
	loaded = true;
}

void ReportSheet::print(std::wostream& output)
{
	for (vector<wstring>::iterator it = sheet_fields.begin(); it != sheet_fields.end(); ++it)
		output << L"\t" << *it;
	output << L"\n";
	for (std::map<std::wstring, Report>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		it->second.print(output, sheet_fields);
	}
}