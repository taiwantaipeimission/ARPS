#include "ReportCollection.h"
#include <map>
#include <sstream>

#include "utility.h"
#include "File.h"
#include "CompList.h"
#include "Area.h"

ReportCollection::ReportCollection()
{
}


ReportCollection::~ReportCollection()
{
}

void ReportCollection::init(std::wstring global_prefix_in)
{
	global_prefix = global_prefix_in;

	prefix[Report::TYPE_REGULAR] = L"report/report";
	prefix[Report::TYPE_ENGLISH] = L"english/english";
	prefix[Report::TYPE_BAPTISM_RECORD] = L"baptism/baptism_record";
	prefix[Report::TYPE_BAPTISM_SOURCE] = L"baptism/baptism_source";

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

	for (int i = 0; i < Report::NUM_TYPES; i++)
	{
		for (int j = 0; j < NUM_DATA_ORDERS; j++)
		{
			report_files[(Report::Type)i][(DataOrder)j].filepath = global_prefix + prefix[(Report::Type)i] + midfix[(DataOrder)j] + suffix;
		}
	}

	for (int i = 0; i < Report::NUM_TYPES; i++)
	{
		for (int j = 0; j <= INDIV; j++)
		{
			reports[(Report::Type)i][(DataOrder)j].report_type = (Report::Type)i;
		}
	}
}

void ReportCollection::read_report(Report::Type type, DataOrder data_order, File* file)
{
	reports[type][data_order].report_type = type;
	reports[type][data_order].read_stored_all(file->file);
}

void ReportCollection::write_report(Report::Type type, DataOrder data_order, File* file)
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
Report ReportCollection::transform_report(Report rep, DataOrder from, DataOrder to, CompList* comp_list)
{
	if (from == COMP)
	{
		if (to == DISTRICT)
		{
			std::wstring district_name;
			if (comp_list->by_area_name.count(rep.sender_name) > 0)
			{
				district_name = comp_list->by_area_name[rep.sender_name][0].district_name;
			}
			else
			{
				district_name = L"UNKNOWN";
			}
			rep.sender_name = district_name;
		}
		if (to == ZONE)
		{
			std::wstring zone_name;
			if (comp_list->by_area_name.count(rep.sender_name) > 0)
			{
				if (rep.sender_name == L"SANXIA_A")
					int x = 0;
				zone_name = rep.type == Report::TYPE_ENGLISH ? comp_list->by_area_name[rep.sender_name][0].english_unit_name : comp_list->by_area_name[rep.sender_name][0].zone_name;
			}
			else
			{
				zone_name = L"UNKNOWN";
			}
			rep.sub_id = 0;
			rep.sender_name = zone_name;
		}
		else if (to == WARD)
		{
			std::wstring ward_name;
			if (comp_list->by_area_name.count(rep.sender_name) > 0)
			{
				ward_name = comp_list->by_area_name[rep.sender_name][0].ward_name;
			}
			else
			{
				ward_name = L"UNKNOWN";
			}
			rep.sender_name = ward_name;
		}
		else if (to == STAKE)
		{
			std::wstring stake_name;
			if (comp_list->by_area_name.count(rep.sender_name) > 0)
			{
				stake_name = comp_list->by_area_name[rep.sender_name][0].stake_name;
			}
			else
			{
				stake_name = L"UNKNOWN";
			}
			rep.sender_name = stake_name;
		}
		else if (to == MISSION)
		{
			rep.sender_name = L"MISSION";
		}
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

void ReportCollection::total_reports(Report::Type type, DataOrder from, DataOrder to, CompList* comp_list, std::wstring date)
{
	std::map<std::wstring, Report> reports_to_add;
	std::vector<std::wstring> tokens = tokenize(date, ':');
	int date_month = _wtoi(tokenize(date, L':')[1].c_str());

	for (std::map<std::wstring, Report>::iterator it = reports[type][from].reports.begin(); it != reports[type][from].reports.end(); ++it)
	{
		Report transformed = transform_report(it->second, from, to, comp_list);
		int transformed_date_month = transformed.date_month;

		if ((transformed.sender_name != L"UNKNOWN" && reports[type][to].reports.count(transformed.get_id_str()) <= 0)						//Conditions for writing/overwriting a new report: no existing report
			|| transformed.get_date() == date														//Still receiving reports for today and updating sums
			|| (transformed.date_month == date_month && transformed.date_wday == 0))				//We're adding a monthly report for the current month
		{
			if (reports_to_add.count(transformed.get_id_str()) > 0)
			{
				reports_to_add[transformed.get_id_str()] += transformed;
			}
			else
			{
				reports_to_add[transformed.get_id_str()] = transformed;						//Start adding a new report
			}
		}
	}

	for (std::map<std::wstring, Report>::iterator it = reports_to_add.begin(); it != reports_to_add.end(); ++it)
	{
		reports[type][to].reports[it->first] = it->second;
	}
}

void ReportCollection::total(Report::Type type, CompList* comp_list, std::wstring date)
{
	//Don't total text report
	if (type != Report::TYPE_BAPTISM_RECORD)
	{
		total_reports(type, COMP, DISTRICT, comp_list, date);
		total_reports(type, COMP, ZONE, comp_list, date);
		total_reports(type, COMP, WARD, comp_list, date);
		total_reports(type, COMP, STAKE, comp_list, date);
		total_reports(type, DISTRICT, DISTRICT_MONTH, comp_list, date);
		total_reports(type, ZONE, ZONE_MONTH, comp_list, date);
		total_reports(type, WARD, WARD_MONTH, comp_list, date);
		total_reports(type, STAKE, STAKE_MONTH, comp_list, date);
		total_reports(type, ZONE, MISSION, comp_list, date);
		total_reports(type, MISSION, MISSION_MONTH, comp_list, date);
	}
}

void ReportCollection::total_all(CompList* comp_list, std::wstring date, std::wstring english_date)
{
	total(Report::TYPE_REGULAR, comp_list, date);
	total(Report::TYPE_ENGLISH, comp_list, english_date);
	total(Report::TYPE_BAPTISM_SOURCE, comp_list, date);
}

bool ReportCollection::load_all()
{
	for (int i = 0; i < Report::NUM_TYPES; i++)
	{
		for (int j = 0; j < NUM_DATA_ORDERS; j++)
		{
			//Don't bother with baptism record for district, zone, etc. since it is a text report
			if (i != Report::TYPE_BAPTISM_RECORD || j == COMP)
			{
				if (report_files[(Report::Type)i][(DataOrder)j].open(File::FILE_TYPE_INPUT))
					reports[(Report::Type)i][(DataOrder)j].read_stored_all(report_files[(Report::Type)i][(DataOrder)j].file);
				report_files[(Report::Type)i][(DataOrder)j].close();
			}
		}
	}
	return true;
}

bool ReportCollection::save_all()
{
	for (int i = 0; i < Report::NUM_TYPES; i++)
	{
		for (int j = 0; j < NUM_DATA_ORDERS; j++)
		{
			//Don't bother with baptism record for district, zone, etc. since it is a text report
			if (i != Report::TYPE_BAPTISM_RECORD || j == COMP)
			{
				if (reports[(Report::Type)i][(DataOrder)j].changed && report_files[(Report::Type)i][(DataOrder)j].open(File::FILE_TYPE_OUTPUT))
				{
					reports[(Report::Type)i][(DataOrder)j].print(report_files[(Report::Type)i][(DataOrder)j].file);
					reports[(Report::Type)i][(DataOrder)j].changed = false;
				}
				report_files[(Report::Type)i][(DataOrder)j].close();
			}
		}
	}
	return true;
}