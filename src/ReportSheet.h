#pragma once

#include <string>
#include <fstream>

#include "Report.h"

class ReportSheet
{
public:

	Report::Type report_type;

	std::wstring header_row;
	std::map<std::wstring, Report> reports;
	bool loaded;
	
	//Does the report sheet need to be re-saved?
	bool changed;

	ReportSheet();
	virtual ~ReportSheet();

	void add_report(Report report);
	void remove_report(std::wstring id_str);
	void read_stored_all(std::wistream& input);
	void print(std::wostream& output);
};

