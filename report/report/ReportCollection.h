#pragma once

class File;

#include "ReportSheet.h"

class ReportCollection
{
public:
	enum DataOrder
	{
		COMP,					//Data by companionship
		ZONE,					//Data by zone
		ZONE_MONTH,				//Data by zone (monthly totals)
		INDIV					//Data by individual
	};

	std::map<Report::Type, std::map<DataOrder, ReportSheet>> reports;		//Keyed by a combination of report type and data order

	ReportCollection();
	virtual ~ReportCollection();

	void read_report(Report::Type type, DataOrder data_order, File* file);
	void write_report(Report::Type type, DataOrder data_order, File* file);

	void calculate_report_by_zone(Report::Type type, CompList* comp_list, std::wstring date);
	void calculate_report_by_indiv();
};

