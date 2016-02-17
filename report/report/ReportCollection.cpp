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

void ReportCollection::calculate_report_by_zone(Report::Type type, CompList* comp_list, std::wstring date)
{
	//Remove all zone reports for the current date, so they can be updated
	for (std::map<std::wstring, Report>::iterator it = reports[type][ZONE].reports.begin(); it != reports[type][ZONE].reports.end();)
	{
		std::wstring report_date = it->second.get_date();
		if (report_date == date)
			it = reports[type][ZONE].reports.erase(it);
		else
			++it;
	}

	std::map<std::wstring, Report> reports_to_add;

	for (std::map<std::wstring, Report>::iterator it = reports[type][COMP].reports.begin(); it != reports[type][COMP].reports.end(); ++it)
	{
		std::wstring comp_report_date = it->second.get_date();
		if (comp_list->areas.count(it->second.sender_number) > 0)
		{
			std::wstring zone_name = type == Report::TYPE_ENGLISH ? comp_list->areas[it->second.sender_number].english_unit_name : comp_list->areas[it->second.sender_number].zone_name;
			std::wstring zone_id_str = comp_report_date + L":" + zone_name;

			if (reports[type][ZONE].reports.count(zone_id_str) <= 0)			//There is no report for this zone yet
			{
				if (reports_to_add.count(zone_id_str) > 0)
				{
					reports_to_add[zone_id_str] += it->second;
				}
				else
				{
					Report report = it->second;
					report.id_str = zone_id_str;
					reports_to_add[report.id_str] = report;						//Start adding a new report
				}
			}
		}
	}

	for (std::map<std::wstring, Report>::iterator it = reports_to_add.begin(); it != reports_to_add.end(); ++it)
	{
		reports[type][ZONE].reports[it->first] = it->second;
	}

	reports_to_add.clear();

	for (std::map<std::wstring, Report>::iterator it = reports[type][ZONE_MONTH].reports.begin(); it != reports[type][ZONE_MONTH].reports.end();)
	{
		std::wstring report_date = it->second.get_date();
		if (report_date == date)
			it = reports[type][ZONE_MONTH].reports.erase(it);
		else
			++it;
	}

	for (std::map<std::wstring, Report>::iterator it = reports[type][ZONE].reports.begin(); it != reports[type][ZONE].reports.end(); ++it)
	{
		std::wstring report_date = it->second.get_date();
		std::wstring zone_name = it->second.get_sender_name();
		std::wstring report_date_year_month = report_date.substr(0, report_date.find(L":", report_date.find(L":") + 1));

		std::wstring zone_id_str = report_date_year_month + L":0:0:" + zone_name;

		if (reports[type][ZONE_MONTH].reports.count(zone_id_str) <= 0)
		{
			if (reports_to_add.count(zone_id_str) > 0)
			{
				reports_to_add[zone_id_str] += it->second;
			}
			else
			{
				Report report = it->second;
				report.id_str = zone_id_str;
				reports_to_add[report.id_str] = report;
			}
		}
	}

	for (std::map<std::wstring, Report>::iterator it = reports_to_add.begin(); it != reports_to_add.end(); ++it)
	{
		reports[type][ZONE_MONTH].reports[it->first] = it->second;
	}
}

void ReportCollection::calculate_report_by_indiv()
{

}