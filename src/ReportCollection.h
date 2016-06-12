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

class File;
class CompList;

#include "ReportSheet.h"
#include <string>

class ReportCollection
{
public:
	

	std::wstring global_prefix;
	std::map<ReportType, std::wstring> prefix;
	std::map<ReportOrder, std::wstring> midfix;
	std::map<ReportType, std::vector<ReportOrder>> orders_to_sum;
	
	std::wstring suffix;
	std::map<ReportType, std::map<ReportOrder, ReportSheet>> reports;		//Keyed by a combination of report type and data order
	std::map<ReportType, std::map<ReportOrder, File>> report_files;			//File paths; organized same as report sheets above

	ReportCollection();
	virtual ~ReportCollection();

	void init(std::wstring global_prefix_in);

	bool is_saved();
	bool load();
	bool save();
	void create_baptism_source_reports(std::wstring date);
	void create_baptism_history_reports(std::wstring date);
	void total_type(ReportType type, CompList* comp_list, std::wstring date);

	bool can_add_report(ReportType type, ReportOrder order, std::wstring date, Report report);
	void read_report(ReportType type, ReportOrder data_order, File* file);
	void write_report(ReportType type, ReportOrder data_order, File* file);
	Report transform_report(Report rep, ReportType type, ReportOrder from, ReportOrder to, CompList* comp_list);
	void total_reports(ReportType type, ReportOrder from, ReportOrder to, CompList* comp_list, std::wstring date);
};

