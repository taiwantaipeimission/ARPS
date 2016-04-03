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
	: report_type(TYPE_REGULAR), report_order(COMP), reports(), changed(false), loaded(false), use_sub_ids(false)
{
}


ReportSheet::~ReportSheet()
{
}

void ReportSheet::insert_report(Report report)
{
	if (report.get_id_str(use_sub_ids) != L"")
	{
		if (use_sub_ids)
		{
			bool unique_fields_same = false;
			while (reports.count(report.get_id_str(true)) > 0 && !unique_fields_same)
			{
				unique_fields_same = true;
				Report other = reports[report.get_id_str(true)];
				for (vector<pair<wstring, bool>>::iterator it = sheet_fields.begin(); it != sheet_fields.end(); ++it)
				{
					if (it->second && (other.report_values[it->first] != L"" && other.report_values[it->first] != report.report_values[it->first]))
					{
						unique_fields_same = false;
						break;
					}
				}
				if(!unique_fields_same)
					report.sub_id++;
			}
		}
		reports[report.get_id_str(use_sub_ids)] = report;
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

void ReportSheet::load(std::wistream& input)
{
	input.clear();
	input.seekg(0, std::ios::beg);
	if (input.good())
	{
		wchar_t line[2028] = L"";
		wstring line_s = L"";

		input.getline(line, 2028, '\n');
		vector<wstring> sheet_field_strings = tokenize(line, L'\t');
		for (vector<wstring>::iterator it = sheet_field_strings.begin() + 1; it != sheet_field_strings.end(); ++it)	//+1 to ignore the first column, which is blank (contains ID string)
		{
			wstring field = *it;
			bool is_unique = field.find('@') != wstring::npos;
			strip_chars(field, L"@");
			sheet_fields.push_back(pair<wstring, bool>(field, is_unique));
			if (is_unique && report_order == COMP)
				use_sub_ids = true;
		}
		
		while (input.good())
		{
			input.get(line, 2028, '\n');
			input.ignore(1, '\n');
			line_s = line;

			if (line_s.length() > 0)
			{
				Report report;
				report.read_processed(line_s, sheet_fields);
				insert_report(report);
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
	for (vector<pair<wstring, bool>>::iterator it = sheet_fields.begin(); it != sheet_fields.end(); ++it)
		output << L"\t" << (it->second ? L"@" : L"") << it->first;
	output << L"\n";
	for (std::map<std::wstring, Report>::iterator it = reports.begin(); it != reports.end(); ++it)
	{
		it->second.print(output, sheet_fields, use_sub_ids);
	}
	changed = false;
}