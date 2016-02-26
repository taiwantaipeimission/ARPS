#pragma once

class File;

#include "ReportSheet.h"
#include <string>

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
		INDIV,					//Data by individual
		NUM_DATA_ORDERS
	};
	std::map<Report::Type, std::wstring> prefix;
	std::map<DataOrder, std::wstring> midfix;
	std::wstring suffix;
	std::map<Report::Type, std::map<DataOrder, ReportSheet>> reports;		//Keyed by a combination of report type and data order
	std::map<Report::Type, std::map<DataOrder, File>> report_files;			//File paths; organized same as report sheets above

	ReportCollection();
	virtual ~ReportCollection();

	bool load_all();
	bool save_all();
	void total_all(CompList* comp_list, std::wstring date, std::wstring english_date);

	void total(Report::Type type, CompList* comp_list, std::wstring date);
	void read_report(Report::Type type, DataOrder data_order, File* file);
	void write_report(Report::Type type, DataOrder data_order, File* file);
	Report transform_report(Report rep, DataOrder from, DataOrder to, CompList* comp_list);
	void total_reports(Report::Type type, DataOrder from, DataOrder to, CompList* comp_list, std::wstring date);
};

