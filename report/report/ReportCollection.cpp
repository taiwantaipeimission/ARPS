#include "ReportCollection.h"
#include <map>
#include <sstream>

#include "File.h"
#include "CompList.h"
#include "Area.h"

ReportCollection::ReportCollection()
	: reports()
{
	for (int i = 0; i <= Report::TYPE_BAPTISM_SOURCE; i++)
	{
		for (int j = 0; j <= INDIV; j++)
		{
			reports[(Report::Type)i][(DataOrder)j].report_type = (Report::Type)i;
		}
	}
}


ReportCollection::~ReportCollection()
{
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

 	COMP->ZONE
	COMP->STAKE
	COMP->MISSION

	ZONE->ZONE_MONTH
	ZONE->MISSION

	STAKE->STAKE_MONTH
	STAKE->MISSION

	MISSION->MISSION_MONTH
*/
std::wstring ReportCollection::get_owner_id_str(Report* rep, DataOrder from, DataOrder to, CompList* comp_list)
{
	if (from == COMP)
	{
		if (to == ZONE)
		{
			std::wstring zone_name = L"UNKNOWN";
			if (comp_list->areas.count(rep->sender_number) > 0)
			{
				zone_name = rep->type == Report::TYPE_ENGLISH ? comp_list->areas[rep->sender_number].english_unit_name : comp_list->areas[rep->sender_number].zone_name;
			}
			return (rep->get_date() + L":" + zone_name);
		}
		else if (to == STAKE)
		{
			std::wstring stake_name = L"UNKNOWN";
			if (comp_list->areas.count(rep->sender_number) > 0)
			{
				stake_name = comp_list->areas[rep->sender_number].stake_name;
			}
			return (rep->get_date() + L":" + stake_name);
		}
		else if (to == MISSION)
		{
			return (rep->get_date() + L":" + L"MISSION");
		}
	}
	else if (from == ZONE)
	{
		if (to == ZONE_MONTH)
		{
			std::wstring report_date = rep->get_date();
			std::wstring zone_name = rep->get_sender_name();
			std::wstring report_date_year_month = report_date.substr(0, report_date.find(L":", report_date.find(L":") + 1));
			return (report_date_year_month + L":0:0:" + zone_name);
		}
		else if (to == MISSION)
		{
			return (rep->get_date() + L":" + L"MISSION");
		}
	}
	else if (from == STAKE)
	{
		if (to == STAKE_MONTH)
		{
			std::wstring report_date = rep->get_date();
			std::wstring stake_name = rep->get_sender_name();
			std::wstring report_date_year_month = report_date.substr(0, report_date.find(L":", report_date.find(L":") + 1));
			return (report_date_year_month + L":0:0:" + stake_name);
		}
		else if (to == MISSION)
		{
			return (rep->get_date() + L":" + L"MISSION");
		}
	}
	else if (from == MISSION)
	{
		if (to == MISSION_MONTH)
		{
			std::wstring report_date = rep->get_date();
			std::wstring zone_name = rep->get_sender_name();
			std::wstring report_date_year_month = report_date.substr(0, report_date.find(L":", report_date.find(L":") + 1));
			return (report_date_year_month + L":0:0:MISSION");
		}
	}
}

void ReportCollection::total_reports(Report::Type type, DataOrder from, DataOrder to, CompList* comp_list, std::wstring date)
{
	//Remove all owner reports for the current date, so they can be updated
	for (std::map<std::wstring, Report>::iterator it = reports[type][to].reports.begin(); it != reports[type][to].reports.end();)
	{
		std::wstring report_date = it->second.get_date();
		if (report_date == date)
			it = reports[type][to].reports.erase(it);
		else
			++it;
	}

	std::map<std::wstring, Report> reports_to_add;

	for (std::map<std::wstring, Report>::iterator it = reports[type][from].reports.begin(); it != reports[type][from].reports.end(); ++it)
	{
		std::wstring comp_report_date = it->second.get_date();
		if (comp_list->areas.count(it->second.sender_number) > 0)
		{
			std::wstring new_id_str = get_owner_id_str(&it->second, from, to, comp_list);

			if (reports[type][to].reports.count(new_id_str) <= 0)			//There is no report entered yet
			{
				if (reports_to_add.count(new_id_str) > 0)
				{
					reports_to_add[new_id_str] += it->second;
				}
				else
				{
					Report report = it->second;
					report.id_str = new_id_str;
					reports_to_add[report.id_str] = report;						//Start adding a new report
				}
			}
		}
	}

	for (std::map<std::wstring, Report>::iterator it = reports_to_add.begin(); it != reports_to_add.end(); ++it)
	{
		reports[type][to].reports[it->first] = it->second;
	}
}

void ReportCollection::total_all_reports(Report::Type type, CompList* comp_list, std::wstring date)
{
	total_reports(type, COMP, ZONE, comp_list, date);
	total_reports(type, COMP, STAKE, comp_list, date);
	total_reports(type, ZONE, ZONE_MONTH, comp_list, date);
	total_reports(type, ZONE, MISSION, comp_list, date);
	total_reports(type, STAKE, STAKE_MONTH, comp_list, date);
	total_reports(type, MISSION, MISSION_MONTH, comp_list, date);
}