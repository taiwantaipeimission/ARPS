#pragma once

#include <string>
#include <fstream>

#include "ReportRegular.h"

class ReportSheet
{
public:
	

	std::string year;	//ex: 2016
	std::string month;	//<1-12>
	std::string week;	//<1-4>
	std::string day;	//<1-7>

	std::map<std::string, ReportRegular> reports;

	ReportSheet(std::string year_in, std::string month_in, std::string week_in, std::string day_in);
	virtual ~ReportSheet();

	void add_report(ReportRegular report);
	void read_unprocessed(std::istream& input);
	void read_processed(std::istream& input);
	void print(std::ofstream& output);
};

