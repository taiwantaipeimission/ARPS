#pragma once

#include <string>
#include <map>
#include <vector>
#include <fstream>

#include "CompList.h"
#include "Message.h"

class Report
{
public:
	std::string id_str;
	std::string sender_number;
	bool is_new;

	const static std::string key_list_array[];
	const static int num_keys;
	std::map<std::string, std::string> report_values;

	Report();
	Report(Message msg, std::string date);
	virtual ~Report();

	std::string get_id_str();
	std::string get_date();
	std::string get_sender_name();

	bool operator==(Report& other);
	bool operator!=(Report& other);

	void add_field(std::string key, std::string value);
	void remove_field(std::string key);

	void read_processed(std::string input);
	void print(std::ostream& output);
};