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

#include "ReportSheet.h"
#include <string>

class ReportCollection
{
public:
	enum DataOrder
	{
		COMP,					//Data by companionship
		DISTRICT,				//Data by district
		DISTRICT_MONTH,			//Data by district (monthly totals)
		ZONE,					//Data by zone
		ZONE_MONTH,				//Data by zone (monthly totals)
		WARD,					//Ward
		WARD_MONTH,
		STAKE,
		STAKE_MONTH,
		MISSION,
		MISSION_MONTH,
		INDIV,					//Data by individual
		NUM_DATA_ORDERS
	};

	std::wstring global_prefix;
	std::map<Report::Type, std::wstring> prefix;
	std::map<DataOrder, std::wstring> midfix;
	std::wstring suffix;
	std::map<Report::Type, std::map<DataOrder, ReportSheet>> reports;		//Keyed by a combination of report type and data order
	std::map<Report::Type, std::map<DataOrder, File>> report_files;			//File paths; organized same as report sheets above
	bool loaded_base;
	bool loaded_aux;

	ReportCollection();
	virtual ~ReportCollection();

	void init(std::wstring global_prefix_in);

	bool is_saved();
	bool load(bool base, bool aux);
	bool save_all();
	void create_baptism_source_reports();
	void total_all(CompList* comp_list, std::wstring date, std::wstring english_date);

	void total(Report::Type type, CompList* comp_list, std::wstring date);
	void read_report(Report::Type type, DataOrder data_order, File* file);
	void write_report(Report::Type type, DataOrder data_order, File* file);
	Report transform_report(Report rep, DataOrder from, DataOrder to, CompList* comp_list);
	void total_reports(Report::Type type, DataOrder from, DataOrder to, CompList* comp_list, std::wstring date);
};

