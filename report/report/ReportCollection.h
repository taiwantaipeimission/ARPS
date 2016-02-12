#pragma once

class File;

#include "ReportSheet.h"

class ReportCollection
{
public:
	ReportSheet report_by_comp;				//One report per companionship
	ReportSheet report_by_zone;				//One report per zone
	ReportSheet report_by_zone_month;		//One report per zone, contains monthly totals to date
	ReportSheet report_by_indiv;			//One report per missionary

	ReportCollection();
	virtual ~ReportCollection();

	void read_report_by_comp(File* file);

	void write_report_by_comp(File* file);
	void write_report_by_zone(File* file);
	void write_report_by_zone_month(File* file);
	void write_report_by_indiv(File* file);

	void calculate_report_by_zone(CompList* comp_list, std::wstring date, bool english = false);
	void calculate_report_by_indiv();
};

