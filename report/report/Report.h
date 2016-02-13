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
		TYPE_ENGLISH,
		TYPE_BAPTISM_RECORD,
		TYPE_BAPTISM_SOURCE
	};

	Type type;
	std::wstring id_str;
	std::wstring sender_number;
	bool is_new;

	std::vector<std::wstring> key_list;
	std::map<std::wstring, std::wstring> report_values;

	Report();
	virtual ~Report();

	virtual void set_type(Type type);
	virtual void read_message(Message msg, std::wstring date);

	

	virtual std::wstring get_id_str();
	virtual std::wstring get_date();
	virtual std::wstring get_sender_name();

	virtual bool operator==(Report& other);
	virtual bool operator!=(Report& other);
	virtual void operator+=(Report& other);

	virtual void add_field(std::wstring key, std::wstring value);
	virtual void remove_field(std::wstring key);

	virtual void read_processed(std::wstring input);
	virtual void print(std::wostream& output);
};