#pragma once

#include <string>
#include <map>
#include <vector>
#include <fstream>

#include "CompList.h"

class ReportRegular
{
public:
	std::string id_str;

	std::vector<std::string> key_list;
	std::map<std::string, std::string> report_values;

	ReportRegular();
	virtual ~ReportRegular();

	std::string get_id_str();

	bool operator==(ReportRegular& other);
	bool operator!=(ReportRegular& other);

	void add_field(std::string key, std::string value);
	void remove_field(std::string key);

	bool read_unprocessed(std::string input, std::string date, CompList* comp_list = NULL);
	void read_processed(std::string input);
	void print(std::ostream& output);

	
};