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
	enum Type
	{
		TYPE_REGULAR,
		TYPE_ENGLISH
	};

	Type type;
	std::wstring id_str;
	std::wstring sender_number;
	bool is_new;

	std::vector<std::wstring> key_list;
	std::map<std::wstring, std::wstring> report_values;

	Report();
	virtual ~Report();

	virtual void set_key_list();	//Initializes the key list for a specific type of report
	virtual void read_message(Message msg, std::wstring date);

	std::wstring get_id_str();
	std::wstring get_date();
	std::wstring get_sender_name();

	bool operator==(Report& other);
	bool operator!=(Report& other);

	void add_field(std::wstring key, std::wstring value);
	void remove_field(std::wstring key);

	void read_processed(std::wstring input);
	void print(std::wostream& output);
};