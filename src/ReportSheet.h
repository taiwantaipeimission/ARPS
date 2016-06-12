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
private:
	ReportType report_type;
	ReportOrder report_order;

	std::vector<std::pair<std::wstring, bool>> sheet_fields;	// wstring: field name; bool: is unique field (prefaced by @)
	std::map<std::wstring, Report> reports;
	bool loaded;
	bool use_sub_ids;
	//Does the report sheet need to be re-saved?
	bool changed;

public:
	ReportSheet();
	virtual ~ReportSheet();

	ReportType get_report_type() { return report_type; }
	ReportOrder get_report_order() { return report_order; }
	std::vector<std::pair<std::wstring, bool>> get_sheet_fields() { return sheet_fields; }
	std::map<std::wstring, Report>* get_reports() { return &reports; }
	Report get_report(std::wstring key) { return reports.count(key) > 0 ? reports[key] : Report(); }

	void insert_report(Report report);
	void remove_report(std::wstring id_str);
	void clear_all_reports();
	void set_sheet_fields(std::vector<std::pair<std::wstring, bool>> sheet_fields_in) { sheet_fields = sheet_fields_in; changed = true; }
	void set_report_type(ReportType report_type_in) { report_type = report_type_in; changed = true; }
	void set_report_order(ReportOrder report_order_in) { report_order = report_order_in; changed = true; }

	bool is_loaded() { return loaded; }
	bool is_changed() { return changed; }
	bool uses_sub_ids() { return use_sub_ids; }

	void load(std::wistream& input);
	void print(std::wostream& output);
};

