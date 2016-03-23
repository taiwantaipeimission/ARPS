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
		TYPE_BAPTISM_SOURCE,
		NUM_TYPES
	};

	Type type;
	
	int date_year;
	int date_month;
	int date_week;
	int date_wday;
	std::wstring sender_name;
	int sub_id;							//Used to differentiate English reports for different class levels, same date, same companionship.  Other report types don't use
	bool use_sub_id;
	bool is_new;

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

	virtual void clear_values();

	virtual void read_processed(std::wstring input, std::vector<std::wstring> header_tokens);
	virtual void print(std::wostream& output);
};