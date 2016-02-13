#include "ReportCollection.h"
#include <map>
#include <sstream>

#include "File.h"
#include "CompList.h"
#include "Area.h"

ReportCollection::ReportCollection()
	: report_by_comp(), report_by_zone(), report_by_zone_month(), report_by_indiv()
{
}


ReportCollection::~ReportCollection()
{
}

void ReportCollection::read_report_by_comp(File* file)
{
	report_by_comp.read_stored_all(file->file);
	report_by_zone.header_row = report_by_comp.header_row;
	report_by_zone_month.header_row = report_by_comp.header_row;
	report_by_indiv.header_row = report_by_comp.header_row;

	report_by_zone.reports.clear();
	report_by_indiv.reports.clear();
}

void ReportCollection::write_report_by_comp(File* file)
{
	report_by_comp.print(file->file);
}

void ReportCollection::write_report_by_zone(File* file)
{
	report_by_zone.print(file->file);
}

void ReportCollection::write_report_by_zone_month(File* file)
{
	report_by_zone_month.print(file->file);
}

void ReportCollection::write_report_by_indiv(File* file)
{
	report_by_indiv.print(file->file);
}

void ReportCollection::calculate_report_by_zone(CompList* comp_list, std::wstring date, bool english)
{
	report_by_zone.reports.clear();
	for (std::map<std::wstring, Report>::iterator it = report_by_comp.reports.begin(); it != report_by_comp.reports.end(); ++it)
	{
		std::wstring comp_report_date = it->second.get_date();
		if (comp_list->areas.count(it->second.sender_number) > 0)
		{
			std::wstring zone_name = english ? comp_list->areas[it->second.sender_number].english_unit_name : comp_list->areas[it->second.sender_number].zone_name;
			std::wstring zone_id_str = comp_report_date + L":" + zone_name;

			if (report_by_zone.reports.count(zone_id_str) > 0)
			{
				Report zone_report = report_by_zone.reports[zone_id_str];
				zone_report += it->second;
				report_by_zone.add_report(zone_report);
			}
			else
			{
				Report report = it->second;
				report.id_str = zone_id_str;
				report_by_zone.add_report(report);
			}
		}
	}

	report_by_zone_month.reports.clear();

	for (std::map<std::wstring, Report>::iterator it = report_by_zone.reports.begin(); it != report_by_zone.reports.end(); ++it)
	{
		std::wstring report_date = it->second.get_date();
		std::wstring zone_name = it->second.get_sender_name();
		std::wstring report_date_year_month = report_date.substr(0, report_date.find(L":", report_date.find(L":") + 1));

		std::wstring zone_id_str = report_date_year_month + L":0:0:" + zone_name;

		if (report_by_zone_month.reports.count(zone_id_str) > 0)
		{
			Report monthly_zone_report = report_by_zone_month.reports[zone_id_str];
			monthly_zone_report += it->second;
			report_by_zone_month.add_report(monthly_zone_report);
		}
		else
		{
			Report report = it->second;
			report.id_str = zone_id_str;
			report_by_zone_month.add_report(report);
		}
	}
}

void ReportCollection::calculate_report_by_indiv()
{

}