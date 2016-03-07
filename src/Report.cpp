#include "Report.h"

#include <sstream>
#include <algorithm>

#include "utility.h"
#include "Message.h"

Report::Report()
{
	set_type(TYPE_REGULAR);
}

void Report::read_message(Message msg, std::wstring date)
{
	if (msg.type == TYPE_REPORT)
		set_type(TYPE_REGULAR);
	else if (msg.type == TYPE_REPORT_ENGLISH)
		set_type(TYPE_ENGLISH);
	else if (msg.type == TYPE_REPORT_BAPTISM)
		set_type(TYPE_BAPTISM_RECORD);

	sender_name = msg.sender_name;
	std::vector<std::wstring> date_strs = tokenize(date, ':');
	if (date_strs.size() >= 4)
	{
		date_year = _wtoi(date_strs[0].c_str());
		date_month = _wtoi(date_strs[1].c_str());
		date_week = _wtoi(date_strs[2].c_str());
		date_wday = _wtoi(date_strs[3].c_str());
	}

	std::wstring value;

	for (unsigned int i = 0; i < int_key_list.size(); i++)
	{
		int_values[int_key_list[i]] = _wtoi(get_msg_key_val(msg.contents, int_key_list[i], ':', '\n').c_str());
	}
	for (unsigned int i = 0; i < string_key_list.size(); i++)
	{
		string_values[string_key_list[i]] = get_msg_key_val(msg.contents, string_key_list[i], ':', '\n');
	}
}

Report::~Report()
{
}

void Report::set_type(Type new_type)
{
	type = new_type;
	use_sub_id = false;
	if (type == TYPE_REGULAR)
	{
		string_key_list = {};
		int_key_list = { L"A", L"B", L"C", L"D", L"NEXTWEEKBAP", L"BAP", L"CONF", L"BD", L"SAC", L"PK", L"OL", L"NIMISSFIND", L"NIMEMREF", L"RCLA", L"LAC", L"RCT" };
	}
	else if (type == TYPE_ENGLISH)
	{
		use_sub_id = true;
		string_key_list = { L"CLASSLEVEL" };
		int_key_list = { L"TOTALSTUDENTS", L"TOTALNONMEM", L"NEWSTUDENTS", L"NEWINV" };
	}
	else if (type == TYPE_BAPTISM_RECORD)
	{
		use_sub_id = true;
		string_key_list = { L"CONV_NAME", L"BP_DATE", L"CONF_DATE", L"WARD", L"HOME_ADDR", L"PH_NUM"};
		int_key_list = { L"BAP_SOURCE" };
	}
	else if (type == TYPE_BAPTISM_SOURCE)
	{
		string_key_list = {};
		int_key_list = { L"BAP_MISS_FIND", L"BAP_LA_REF", L"BAP_RC_REF", L"BAP_MEM_REF", L"BAP_ENGLISH", L"BAP_TOUR" };
	}
}

std::wstring Report::get_id_str()
{
	return get_date() + L":" + (use_sub_id ? tos(sub_id) + L":" + sender_name : sender_name);
}

std::wstring Report::get_date()
{
	std::wstring date = tos(date_year) + L":" + tos(date_month) + L":" + tos(date_week) + L":" + tos(date_wday);
	return date;
}

std::wstring Report::get_sender_name()
{
	return sender_name;
}

bool Report::operator==(Report& other)
{
	if (other.get_id_str() != this->get_id_str())
		return false;
	
	// sender number and cmgl_id don't matter; not retained

	if (this->string_values != other.string_values || this->int_values != other.int_values)
		return false;


	return true;
}

bool Report::operator!=(Report& other)
{
	return !((*this)==other);
}

void Report::operator+=(Report& other)
{
	for (std::map<std::wstring, int>::iterator i = other.int_values.begin(); i != other.int_values.end(); ++i)
	{
		if (this->int_values.count(i->first) > 0)	//Existing value for this report field
		{
			this->int_values[i->first] += i->second;
		}
		else
		{
			this->add_int(i->first, i->second);
		}
	}
}

void Report::add_int(std::wstring key, int value)
{
	int_values[key] = value;
}

void Report::add_string(std::wstring key, std::wstring value)
{
	string_values[key] = value;
}

void Report::remove_int(std::wstring key)
{
	int_values.erase(key);
}

void Report::remove_string(std::wstring key)
{
	string_values.erase(key);
}

void Report::clear_values()
{
	string_values.clear();
	int_values.clear();
}

void Report::read_processed(std::wstring input)
{
	std::wstringstream stream(input);
	std::wstring id_str;
	stream >> id_str;

	sender_name = L"-";

	//Tokenize the ID string to get the date stamp, name, and sub-ID if applicable
	std::vector<std::wstring> id_str_tokens = tokenize(id_str, ':');
	if (id_str_tokens.size() >= 5)
	{
		int i = 0;
		date_year = _wtoi(id_str_tokens[i++].c_str());
		date_month = _wtoi(id_str_tokens[i++].c_str());
		date_week = _wtoi(id_str_tokens[i++].c_str());
		date_wday = _wtoi(id_str_tokens[i++].c_str());
		if (id_str_tokens.size() == 6)
		{
			use_sub_id = true;
			sub_id = _wtoi(id_str_tokens[i++].c_str());
		}
		sender_name = id_str_tokens[i++];
	}

	//Extract all of the keyed report values
	for (int i = 0; i < string_key_list.size(); i++)
	{
		std::wstring value = L"0";
		if (stream.good())
			stream >> value;
		string_values[string_key_list[i]] = value;
	}
	for (int i = 0; i < int_key_list.size(); i++)
	{
		int value = 0;
		if (stream.good())
			stream >> value;
		int_values[int_key_list[i]] = value;
	}

	is_new = false;

	return;
}

void Report::print(std::wostream& output)
{
	output << get_id_str();
	for (int i = 0; i < string_key_list.size(); i++)
	{
		output << L'\t' << string_values[string_key_list[i]];
	}
	for (int i = 0; i < int_key_list.size(); i++)
	{
		output << L'\t' << int_values[int_key_list[i]];
	}
	output << L'\n';
}