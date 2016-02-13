#pragma once

#include <string>
#include <fstream>

#include "Report.h"

class ReportSheet
{
public:

	Report::Type report_type;

	std::map<std::wstring, Report> reports;
	std::wstring header_row;

	ReportSheet();
	virtual ~ReportSheet();

	void add_report(Report report);
	void remove_report(std::wstring id_str);
	void read_stored_all(std::wistream& input);
	void print(std::wostream& output);
};

