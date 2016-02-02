#pragma once

#include <string>
#include <fstream>

#include "Report.h"

class ReportSheet
{
public:

	Report::Type report_type;

	std::map<std::string, Report*> reports;
	std::string header_row;

	ReportSheet();
	virtual ~ReportSheet();

	void add_report(Report* report);
	void remove_report(std::string id_str);
	void read_stored_all(std::istream& input);
	void print(std::ostream& output);
};

