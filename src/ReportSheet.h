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

#pragma once

#include <string>
#include <fstream>

#include "Report.h"

class ReportSheet
{
public:

	ReportType report_type;

	std::vector<std::wstring> sheet_fields;
	std::map<std::wstring, Report> reports;
	bool loaded;
	
	//Does the report sheet need to be re-saved?
	bool changed;

	ReportSheet();
	virtual ~ReportSheet();

	void add_report(Report report);
	void remove_report(std::wstring id_str);

	void read_stored_all(std::wistream& input);
	void print(std::wostream& output);
};

