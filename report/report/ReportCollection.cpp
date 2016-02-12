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
	for (std::map<std::wstring, Report*>::iterator it = report_by_comp.reports.begin(); it != report_by_comp.reports.end(); ++it)
	{
		std::wstring comp_report_date = it->second->get_date();
		if (comp_list->areas.count(it->second->sender_number) > 0)
		{
			std::wstring zone_name = english ? comp_list->areas[it->second->sender_number].english_unit_name : comp_list->areas[it->second->sender_number].zone_name;
			std::wstring zone_id_str = comp_report_date + L":" + zone_name;

			if (report_by_zone.reports.count(zone_id_str) > 0)
			{
				Report* zone_report = report_by_zone.reports[zone_id_str];
				for (std::map<std::wstring, std::wstring>::iterator j = it->second->report_values.begin(); j != it->second->report_values.end(); ++j)
				{
					if (zone_report->report_values.count(j->first) > 0)
					{
						std::wstringstream zone_report_ss;
						std::wstringstream comp_report_ss;

						zone_report_ss.str(zone_report->report_values[j->first]);
						comp_report_ss.str(j->second);
						int i_zone_report;
						int i_comp_report;
						zone_report_ss >> i_zone_report;
						comp_report_ss >> i_comp_report;

						if (!zone_report_ss.fail() && !comp_report_ss.fail())
						{
							int new_zone_value = i_zone_report + i_comp_report;
							wchar_t intstr[8];
							_itow_s(new_zone_value, intstr, 8, 10);
							zone_report->report_values[j->first] = std::wstring(intstr);
						}
					}
					else
					{
						zone_report->add_field(j->first, j->second);
					}
				}
				report_by_zone.add_report(zone_report);
			}
			else
			{
				Report* zone_report = new Report(*it->second);
				zone_report->id_str = zone_id_str;
				report_by_zone.add_report(zone_report);
			}
		}
	}

	report_by_zone_month.reports.clear();

	for (std::map<std::wstring, Report*>::iterator it = report_by_zone.reports.begin(); it != report_by_zone.reports.end(); ++it)
	{
		std::wstring report_date = it->second->get_date();
		std::wstring zone_name = it->second->get_sender_name();
		std::wstring report_date_year_month = report_date.substr(0, report_date.find(L":", report_date.find(L":") + 1));

		std::wstring zone_id_str = report_date_year_month + L":0:0:" + zone_name;

		if (report_by_zone_month.reports.count(zone_id_str) > 0)
		{
			Report* monthly_zone_report = report_by_zone_month.reports[zone_id_str];
			for (std::map<std::wstring, std::wstring>::iterator j = it->second->report_values.begin(); j != it->second->report_values.end(); ++j)
			{
				if (monthly_zone_report->report_values.count(j->first) > 0)
				{
					std::wstringstream monthly_zone_report_ss;
					std::wstringstream zone_report_ss;

					monthly_zone_report_ss.str(monthly_zone_report->report_values[j->first]);
					zone_report_ss.str(j->second);
					int i_monthly_zone_report;
					int i_zone_report;
					monthly_zone_report_ss >> i_monthly_zone_report;
					zone_report_ss >> i_zone_report;

					if (!monthly_zone_report_ss.fail() && !zone_report_ss.fail())
					{
						int new_monthly_zone_value = i_monthly_zone_report + i_zone_report;
						wchar_t intstr[8];
						_itow_s(new_monthly_zone_value, intstr, 8, 10);
						monthly_zone_report->report_values[j->first] = std::wstring(intstr);
					}
				}
				else
				{
					monthly_zone_report->add_field(j->first, j->second);
				}
			}
			report_by_zone_month.add_report(monthly_zone_report);
		}
		else
		{
			Report* monthly_zone_report = new Report(*it->second);
			monthly_zone_report->id_str = zone_id_str;
			report_by_zone_month.add_report(monthly_zone_report);
		}
	}
}

void ReportCollection::calculate_report_by_indiv()
{

}