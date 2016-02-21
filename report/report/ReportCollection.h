#pragma once

class File;

#include "ReportSheet.h"

class ReportCollection
{
public:
	enum DataOrder
	{
		COMP,					//Data by companionship
		DISTRICT,				//Data by district
		DISTRICT_MONTH,			//Data by district (monthly totals)
		ZONE,					//Data by zone
		ZONE_MONTH,				//Data by zone (monthly totals)
		STAKE,
		STAKE_MONTH,
		MISSION,
		MISSION_MONTH,
		INDIV					//Data by individual
	};

	std::map<Report::Type, std::map<DataOrder, ReportSheet>> reports;		//Keyed by a combination of report type and data order

	ReportCollection();
	virtual ~ReportCollection();

	void read_report(Report::Type type, DataOrder data_order, File* file);
	void write_report(Report::Type type, DataOrder data_order, File* file);
	std::wstring get_owner_id_str(Report* rep, DataOrder from, DataOrder to, CompList* comp_list);

	void total_reports(Report::Type type, DataOrder from, DataOrder to, CompList* comp_list, std::wstring date);
	void total_all_reports(Report::Type type, CompList* comp_list, std::wstring date);
};

