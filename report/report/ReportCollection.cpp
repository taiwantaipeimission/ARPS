#include "ReportCollection.h"
#include <map>

#include "File.h"
#include "CompList.h"
#include "Area.h"

ReportCollection::ReportCollection()
	: report_by_comp(), report_by_zone(), report_by_indiv()
{
}


ReportCollection::~ReportCollection()
{
}

void ReportCollection::read_report_by_comp(File* file)
{
	report_by_comp.read_stored_all(file->file);
	report_by_zone.header_row = report_by_comp.header_row;
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

void ReportCollection::write_report_by_indiv(File* file)
{
	report_by_indiv.print(file->file);
}

void ReportCollection::calculate_report_by_zone(CompList* comp_list, std::string date)
{
	report_by_zone.reports.clear();
	for (std::map<std::string, ReportRegular>::iterator it = report_by_comp.reports.begin(); it != report_by_comp.reports.end(); ++it)
	{
			std::string comp_report_date = it->second.get_date();
			if (comp_report_date == date && comp_list->areas.count(it->second.sender_number) > 0)
			{
				std::string zone_name = comp_list->areas[it->second.sender_number].zone_name;
				std::string zone_id_str = date + ":" + zone_name;

				if (report_by_zone.reports.count(zone_id_str) > 0)
				{
					ReportRegular zone_report = report_by_zone.reports[zone_id_str];
					for (std::map<std::string, std::string>::iterator j = it->second.report_values.begin(); j != it->second.report_values.end(); ++j)
					{
						if (zone_report.report_values.count(j->first) > 0)
						{
							std::string zone_report_value = zone_report.report_values[j->first];
							std::string comp_report_value = j->second;

							int new_zone_value = std::stoi(zone_report_value) + std::stoi(comp_report_value);
							char intstr[8];
							_itoa_s(new_zone_value, intstr, 8, 10);
							zone_report.report_values[j->first] = std::string(intstr);
						}
						else
						{
							zone_report.add_field(j->first, j->second);
						}
					}
					report_by_zone.add_report(zone_report);
				}
				else
				{
					ReportRegular zone_report = it->second;
					zone_report.id_str = zone_id_str;
					report_by_zone.add_report(zone_report);
				}
			}
	}
}

void ReportCollection::calculate_report_by_indiv()
{

}