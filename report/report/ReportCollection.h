#pragma once

class File;

#include "ReportSheet.h"

class ReportCollection
{
public:
	std::map<Report::Type, ReportSheet> report_by_comp;				//One report per companionship
	std::map<Report::Type, ReportSheet> report_by_zone;				//One report per zone
	std::map<Report::Type, ReportSheet> report_by_zone_month;		//One report per zone, contains monthly totals to date
	std::map<Report::Type, ReportSheet> report_by_indiv;			//One report per missionary

	ReportCollection();
	virtual ~ReportCollection();

	void read_report_by_comp(Report::Type type, File* file);

	void write_report_by_comp(Report::Type type, File* file);
	void write_report_by_zone(Report::Type type, File* file);
	void write_report_by_zone_month(Report::Type type, File* file);
	void write_report_by_indiv(Report::Type type, File* file);

	void calculate_report_by_zone(Report::Type type, CompList* comp_list, std::wstring date);
	void calculate_report_by_indiv();
};

