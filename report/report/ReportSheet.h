#pragma once

#include <string>
#include <fstream>

#include "ReportRegular.h"

class ReportSheet
{
public:

	std::map<std::string, ReportRegular> reports;
	std::string header_row;

	ReportSheet();
	virtual ~ReportSheet();

	void add_report(ReportRegular report);
	void read_unprocessed(std::istream& input, std::string date, CompList* comp_list = NULL);
	void read_stored_all(std::istream& input);
	void print(std::ostream& output);
};

