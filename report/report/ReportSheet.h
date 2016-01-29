#pragma once

#include <string>
#include <fstream>

#include "Report.h"

class ReportSheet
{
public:

	std::map<std::string, Report> reports;
	std::string header_row;

	ReportSheet();
	virtual ~ReportSheet();

	void add_report(Report report);
	void read_stored_all(std::istream& input);
	void print(std::ostream& output);
};

