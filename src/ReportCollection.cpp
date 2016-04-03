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

#include "ReportCollection.h"
#include <map>
#include <sstream>
#include <algorithm>

#include "utility.h"
#include "File.h"
#include "CompList.h"
#include "Area.h"

using namespace std;

ReportCollection::ReportCollection()
{
}


ReportCollection::~ReportCollection()
{
}

void ReportCollection::init(std::wstring global_prefix_in)
{
	global_prefix = global_prefix_in;

	prefix[TYPE_REGULAR] = L"report/report";
	prefix[TYPE_ENGLISH] = L"english/english";
	prefix[TYPE_BAPTISM_RECORD] = L"baptism/baptism_record";
	prefix[TYPE_BAPTISM_SOURCE] = L"baptism/baptism_source";
	prefix[TYPE_REFERRAL] = L"referrals/referrals";

	midfix[COMP] = L"";
	midfix[DISTRICT] = L"_district";
	midfix[DISTRICT_MONTH] = L"_district_month";
	midfix[ZONE] = L"_zone";
	midfix[ZONE_MONTH] = L"_zone_month";
	midfix[WARD] = L"_ward";
	midfix[WARD_MONTH] = L"_ward_month";
	midfix[STAKE] = L"_stake";
	midfix[STAKE_MONTH] = L"_stake_month";
	midfix[MISSION] = L"_mission";
	midfix[MISSION_MONTH] = L"_mission_month";
	midfix[INDIV] = L"_indiv";

	suffix = L".txt";

	for (int i = 0; i < NUM_TYPES; i++)
	{
		ReportType type = (ReportType)i;
		for (int j = 0; j < REPORTS_TO_STORE[type].size(); j++)
		{
			ReportOrder order = REPORTS_TO_STORE[type][j];
			reports[type][order].set_report_type(type);
			reports[type][order].set_report_order(order);
			report_files[type][order].filepath = global_prefix + prefix[type] + midfix[order] + suffix;
		}
	}
}

void ReportCollection::read_report(ReportType type, ReportOrder data_order, File* file)
{
	reports[type][data_order].load(file->file);
}

void ReportCollection::write_report(ReportType type, ReportOrder data_order, File* file)
{
	reports[type][data_order].print(file->file);
}

/* Takes the ID string of the current report and returns the string of its owner in the specified data order.

	Valid conversions:

	COMP->DISTRICT
 	COMP->ZONE
	COMP->WARD
	COMP->STAKE
	COMP->MISSION

	DISTRICT->DISTRICT_MONTH
	DISTRICT->MISSION

	ZONE->ZONE_MONTH
	ZONE->MISSION

	WARD->WARD_MONTH
	WARD->MISSION

	STAKE->STAKE_MONTH
	STAKE->MISSION

	MISSION->MISSION_MONTH
*/
Report ReportCollection::transform_report(Report rep, ReportType type, ReportOrder from, ReportOrder to, CompList* comp_list)
{
	if (from == COMP)
	{
		if (to == DISTRICT)
		{	if (comp_list->by_area_name.count(rep.sender_name) > 0)
				rep.sender_name = comp_list->by_area_name[rep.sender_name][0].district_name;
			else
				rep.sender_name = L"UNKNOWN";
		}
		if (to == ZONE)
		{
			if (comp_list->by_area_name.count(rep.sender_name) > 0)
				rep.sender_name = type == TYPE_ENGLISH ? comp_list->by_area_name[rep.sender_name][0].english_unit_name : comp_list->by_area_name[rep.sender_name][0].zone_name;
			else
				rep.sender_name = L"UNKNOWN";
		}
		else if (to == WARD)
		{
			if (comp_list->by_area_name.count(rep.sender_name) > 0)
				rep.sender_name = comp_list->by_area_name[rep.sender_name][0].ward_name;
			else
				rep.sender_name = L"UNKNOWN";
		}
		else if (to == STAKE)
		{
			if (comp_list->by_area_name.count(rep.sender_name) > 0)
				rep.sender_name = comp_list->by_area_name[rep.sender_name][0].stake_name;
			else
				rep.sender_name = L"UNKNOWN";
		}
		else if (to == MISSION)
			rep.sender_name = L"MISSION";
	}
	else if (from == DISTRICT)
	{
		if (to == DISTRICT_MONTH)
		{
			rep.date_week = 0;
			rep.date_wday = 0;
		}
		else if (to == MISSION)
		{
			rep.sender_name = L"MISSION";
		}
	}
	else if (from == ZONE)
	{
		if (to == ZONE_MONTH)
		{
			rep.date_week = 0;
			rep.date_wday = 0;
		}
		else if (to == MISSION)
		{
			rep.sender_name = L"MISSION";
		}
	}
	else if (from == WARD)
	{
		if (to == WARD_MONTH)
		{
			rep.date_week = 0;
			rep.date_wday = 0;
		}
		else if (to == MISSION)
		{
			rep.sender_name = L"MISSION";
		}
	}
	else if (from == STAKE)
	{
		if (to == STAKE_MONTH)
		{
			rep.date_week = 0;
			rep.date_wday = 0;
		}
		else if (to == MISSION)
		{
			rep.sender_name = L"MISSION";
		}
	}
	else if (from == MISSION)
	{
		if (to == MISSION_MONTH)
		{
			rep.date_week = 0;
			rep.date_wday = 0;
		}
	}
	return rep;
}

bool ReportCollection::can_add_report(ReportType type, ReportOrder order, wstring date, Report report)
{
	map<wstring, Report>* destination_report_map = reports[type][order].get_reports();
	int date_month = _wtoi(tokenize(date, L':')[1].c_str());

	return ((report.sender_name != L"UNKNOWN" && destination_report_map->count(report.get_id_str(reports[type][order].uses_sub_ids())) <= 0)						//Conditions for writing/overwriting a new report: no existing report
		|| report.get_date() == date														//Still receiving reports for today and updating sums
		|| (report.date_month == date_month && report.date_wday == 0));				//We're adding a monthly report for the current month
}

void ReportCollection::create_baptism_source_reports(std::wstring report_date)
{
	map<wstring, Report>* report_map = reports[TYPE_BAPTISM_RECORD][COMP].get_reports();
	for (map<wstring, Report>::iterator it = report_map->begin(); it != report_map->end(); ++it)
	{
		Report bap_source = it->second;
		bap_source.report_values[it->second.report_values[REP_KEY_BAP_SOURCE]] = L"1";
		if (can_add_report(TYPE_BAPTISM_SOURCE, COMP, report_date, bap_source))
		{
			reports[TYPE_BAPTISM_SOURCE][COMP].insert_report(bap_source);
		}
	}
}

void ReportCollection::total_reports(ReportType type, ReportOrder from, ReportOrder to, CompList* comp_list, std::wstring date)
{
	//If we maintain a report sheet for this type and both orders to and from
	if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), from) > 0 && count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), to) > 0)
	{
		std::map<std::wstring, Report> reports_to_add;
		

		reports[type][to].set_sheet_fields(reports[type][from].get_sheet_fields());

		map<wstring, Report>* from_report_map = reports[type][from].get_reports();
		map<wstring, Report>* to_report_map = reports[type][to].get_reports();
		for (std::map<std::wstring, Report>::iterator it = from_report_map->begin(); it != from_report_map->end(); ++it)
		{
			Report transformed = transform_report(it->second, type, from, to, comp_list);

			if (type == TYPE_BAPTISM_SOURCE && to == DISTRICT)
				int x = 0;
			if (can_add_report(type, to, date, transformed))
			{
				wstring new_id_str = transformed.get_id_str(reports[type][to].uses_sub_ids());
				if (reports_to_add.count(new_id_str) > 0)
					reports_to_add[transformed.get_id_str(reports[type][to].uses_sub_ids())] += transformed;
				else
					reports_to_add[transformed.get_id_str(reports[type][to].uses_sub_ids())] = transformed;
			}
		}

		for (std::map<std::wstring, Report>::iterator it = reports_to_add.begin(); it != reports_to_add.end(); ++it)
		{
			reports[type][to].insert_report(it->second);
		}
	}
}

void ReportCollection::total_type(ReportType type, CompList* comp_list, std::wstring date)
{
	if(type == TYPE_BAPTISM_SOURCE)
		create_baptism_source_reports(date);

	if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), COMP) > 0)
	{
		if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), DISTRICT) > 0)
			total_reports(type, COMP, DISTRICT, comp_list, date);
		if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), ZONE) > 0)
			total_reports(type, COMP, ZONE, comp_list, date);
		if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), WARD) > 0)
			total_reports(type, COMP, WARD, comp_list, date);
		if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), STAKE) > 0)
			total_reports(type, COMP, STAKE, comp_list, date);
	}
	if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), DISTRICT) > 0)
	{
		if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), DISTRICT_MONTH) > 0)
			total_reports(type, DISTRICT, DISTRICT_MONTH, comp_list, date);
	}
	if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), ZONE) > 0)
	{
		if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), ZONE_MONTH) > 0)
			total_reports(type, ZONE, ZONE_MONTH, comp_list, date);
		if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), MISSION) > 0)
			total_reports(type, ZONE, MISSION, comp_list, date);
	}
	if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), WARD) > 0)
	{
		if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), WARD_MONTH) > 0)
			total_reports(type, WARD, WARD_MONTH, comp_list, date);
	}
	if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), STAKE) > 0)
	{
		if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), STAKE_MONTH) > 0)
			total_reports(type, STAKE, STAKE_MONTH, comp_list, date);
	}
	if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), MISSION) > 0)
	{
		if (count(REPORTS_TO_STORE[type].begin(), REPORTS_TO_STORE[type].end(), MISSION_MONTH) > 0)
			total_reports(type, MISSION, MISSION_MONTH, comp_list, date);
	}
}

/* Loads only the most basic key indicators, those with the data order COMP.
*/
bool ReportCollection::load()
{
	for (int i = 0; i < NUM_TYPES; i++)
	{
		ReportType type = (ReportType)i;
		for (int j = 0; j < REPORTS_TO_STORE[type].size(); j++)
		{
			ReportOrder order = REPORTS_TO_STORE[type][j];
			if (report_files[type][order].open(File::FILE_TYPE_INPUT))
			{
				reports[type][order].load(report_files[type][order].file);
				report_files[type][order].close();
			}
		}
	}
	return true;
}

bool ReportCollection::is_saved()
{
	bool saved = true;
	for (int i = 0; i < NUM_TYPES && saved; i++)
	{
		ReportType type = (ReportType)i;
		for (int j = 0; j < REPORTS_TO_STORE[type].size() && saved; j++)
		{
			ReportOrder order = REPORTS_TO_STORE[type][j];
			if (reports[type][order].is_loaded() && reports[type][order].is_changed())
				saved = false;
		}
	}
	return saved;
}

bool ReportCollection::save()
{
	for (int i = 0; i < NUM_TYPES; i++)
	{
		ReportType type = (ReportType)i;
		for (int j = 0; j < REPORTS_TO_STORE[type].size(); j++)
		{
			ReportOrder order = REPORTS_TO_STORE[type][j];
			if (reports[type][order].is_loaded() && reports[type][order].is_changed() && report_files[type][order].open(File::FILE_TYPE_OUTPUT))
			{
				reports[type][order].print(report_files[type][order].file);
				report_files[type][order].close();
			}
		}
	}
	return true;
}