#pragma once

#include <string>
#include <map>
#include <vector>

class ReportRegular
{
public:
	ReportRegular();
	virtual ~ReportRegular();

	void add_field(std::string key, std::string value);
	void remove_field(std::string key);

	std::string sender_number;
	std::string sender_name;
	std::vector<std::string> key_list;
	std::map<std::string, std::string> report_values;
};