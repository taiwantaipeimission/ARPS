#pragma once

#include <string>
#include <map>
#include <vector>
#include <fstream>

#include "CompList.h"
#include "Message.h"

class ReportRegular
{
public:
	std::string id_str;
	std::string sender_number;
	bool is_new;

	std::vector<std::string> key_list;
	std::map<std::string, std::string> report_values;

	ReportRegular();
	ReportRegular(Message msg, std::string date);
	virtual ~ReportRegular();

	std::string get_id_str();

	bool operator==(ReportRegular& other);
	bool operator!=(ReportRegular& other);

	void add_field(std::string key, std::string value);
	void remove_field(std::string key);

	void read_processed(std::string input);
	void print(std::ostream& output);

	
};