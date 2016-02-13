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

void ReportCollection::read_report_by_comp(Report::Type type, File* file)
{
	report_by_comp[type].report_type = type;
	report_by_zone[type].report_type = type;
	report_by_zone_month[type].report_type = type;
	report_by_indiv[type].report_type = type;

	report_by_comp[type].read_stored_all(file->file);
	report_by_zone[type].reports.clear();
	report_by_zone_month[type].reports.clear();
	report_by_indiv[type].reports.clear();
}

void ReportCollection::write_report_by_comp(Report::Type type, File* file)
{
	report_by_comp[type].print(file->file);
}

void ReportCollection::write_report_by_zone(Report::Type type, File* file)
{
	report_by_zone[type].print(file->file);
}

void ReportCollection::write_report_by_zone_month(Report::Type type, File* file)
{
	report_by_zone_month[type].print(file->file);
}

void ReportCollection::write_report_by_indiv(Report::Type type, File* file)
{
	report_by_indiv[type].print(file->file);
}

void ReportCollection::calculate_report_by_zone(Report::Type type, CompList* comp_list, std::wstring date)
{
	report_by_zone[type].reports.clear();
	for (std::map<std::wstring, Report>::iterator it = report_by_comp[type].reports.begin(); it != report_by_comp[type].reports.end(); ++it)
	{
		std::wstring comp_report_date = it->second.get_date();
		if (comp_list->areas.count(it->second.sender_number) > 0)
		{
			std::wstring zone_name = type == Report::TYPE_ENGLISH ? comp_list->areas[it->second.sender_number].english_unit_name : comp_list->areas[it->second.sender_number].zone_name;
			std::wstring zone_id_str = comp_report_date + L":" + zone_name;

			if (report_by_zone[type].reports.count(zone_id_str) > 0)
			{
				Report zone_report = report_by_zone[type].reports[zone_id_str];
				zone_report += it->second;
				report_by_zone[type].add_report(zone_report);
			}
			else
			{
				Report report = it->second;
				report.id_str = zone_id_str;
				report_by_zone[type].add_report(report);
			}
		}
	}

	report_by_zone_month[type].reports.clear();

	for (std::map<std::wstring, Report>::iterator it = report_by_zone[type].reports.begin(); it != report_by_zone[type].reports.end(); ++it)
	{
		std::wstring report_date = it->second.get_date();
		std::wstring zone_name = it->second.get_sender_name();
		std::wstring report_date_year_month = report_date.substr(0, report_date.find(L":", report_date.find(L":") + 1));

		std::wstring zone_id_str = report_date_year_month + L":0:0:" + zone_name;

		if (report_by_zone_month[type].reports.count(zone_id_str) > 0)
		{
			Report monthly_zone_report = report_by_zone_month[type].reports[zone_id_str];
			monthly_zone_report += it->second;
			report_by_zone_month[type].add_report(monthly_zone_report);
		}
		else
		{
			Report report = it->second;
			report.id_str = zone_id_str;
			report_by_zone_month[type].add_report(report);
		}
	}
}

void ReportCollection::calculate_report_by_indiv()
{

}