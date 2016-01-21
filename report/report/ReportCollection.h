#pragma once

class File;

#include "ReportSheet.h"

class ReportCollection
{
public:
	ReportSheet report_by_comp;		//One report per companionship
	ReportSheet report_by_zone;		//One report per missionary
	ReportSheet report_by_indiv;	//One report per zone

	ReportCollection();
	virtual ~ReportCollection();

	void read_report_by_comp(File* file);

	void write_report_by_comp(File* file);
	void write_report_by_zone(File* file);
	void write_report_by_indiv(File* file);

	void calculate_report_by_zone(CompList* comp_list, std::string date);
	void calculate_report_by_indiv();
};

