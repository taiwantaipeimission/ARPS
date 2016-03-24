#include "Report.h"

#include <sstream>
#include <algorithm>

#include "utility.h"
#include "Message.h"

using namespace std;

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

	for (map<wstring, wstring>::iterator it = report_values.begin(); it != report_values.end(); ++it)
	{
		it->second = get_msg_key_val(msg.contents, it->first, ':', '\n').c_str();
	}
}

Report::~Report()
{
}

void Report::set_type(Type new_type)
{
	type = new_type;
	use_sub_id = false;
	wstring field_list = L"";
	if (type == TYPE_REGULAR)
	{
		field_list = REPORT_FIELDS;
	}
	else if (type == TYPE_ENGLISH)
	{
		field_list = ENGLISH_FIELDS;
		use_sub_id = true;
	}
	else if (type == TYPE_BAPTISM_RECORD)
	{
		field_list = BAPTISM_RECORD_FIELDS;
		use_sub_id = true;
	}
	else if (type == TYPE_BAPTISM_SOURCE)
	{
		field_list = BAPTISM_SOURCE_FIELDS;
	}

	report_values.clear();
	vector<wstring> field_tokens = tokenize(field_list, ',');
	for (vector<wstring>::iterator it = field_tokens.begin(); it != field_tokens.end(); ++it)
	{
		if (report_values.count(*it) <= 0)
		{
			report_values[*it] = L"";
		}
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

	if (this->report_values != other.report_values)
		return false;

	return true;
}

bool Report::operator!=(Report& other)
{
	return !((*this)==other);
}

void Report::operator+=(Report& other)
{
	for (map<wstring, wstring>::iterator it = other.report_values.begin(); it != other.report_values.end(); ++it)
	{
		if (this->report_values.count(it->first) > 0)	//Existing value for this report field
		{
			char ex_str[256];
			strcpy_s(ex_str, tos(this->report_values[it->first]).c_str());
			char* ex_end;
			long ex_val = strtol(ex_str, &ex_end, 10);

			char add_str[256];
			strcpy_s(add_str, tos(it->second).c_str());
			char* add_end;
			long add_val = strtol(add_str, &add_end, 10);

			if ((ex_end != ex_str || strlen(ex_str) == 0) && (add_end != add_str || strlen(add_str) == 0))
			{
				int summed = (int)(ex_val + add_val);
				this->report_values[it->first] = tos(summed);
			}
		}
		else
		{
			this->report_values[it->first] = it->second;
		}
	}
}

void Report::clear_values()
{
	for (map<wstring, wstring>::iterator it = report_values.begin(); it != report_values.end(); ++it)
		it->second = L"";
}

void Report::read_processed(wstring input, vector<wstring> header_tokens)
{
	vector<wstring> tokens = tokenize(input, '\t');
	map<wstring, wstring> read_value_pairs;
	size_t n_pairs = min(tokens.size(), header_tokens.size());
	for (size_t i = 0; i < n_pairs; i++)
	{
		read_value_pairs[header_tokens[i]] = tokens[i];
	}
	if (tokens.size() > 0)
	{
		std::wstring id_str = tokens[0];
		date_year = 0;
		date_month = 0;
		date_week = 0;
		date_wday = 0;
		use_sub_id = false;
		sub_id = 0;
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
		for (map<wstring, wstring>::iterator it = report_values.begin(); it != report_values.end(); ++it)
		{
			it->second = read_value_pairs.count(it->first) > 0 ? read_value_pairs[it->first] : L"";
		}

		is_new = false;
	}
}

void Report::print(std::wostream& output)
{
	output << get_id_str();
	for (map<wstring, wstring>::iterator it = report_values.begin(); it != report_values.end(); ++it)
	{
		output << L'\t' << it->second;
	}
	output << L'\n';
}