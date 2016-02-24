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
	sender_number = msg.sender_number;

	std::wstring value;

	for (unsigned int i = 0; i < key_list.size(); i++)
	{
		int key_pos = msg.contents.find(L"\n" + key_list[i] + L":");
		int value_pos = msg.contents.find(':', key_pos) + 1;
		int value_end_pos = std::min(msg.contents.find('\n', value_pos), msg.contents.find('\r', value_pos));

		value = L"0";
		if (key_pos != std::wstring::npos && value_pos != std::wstring::npos)
		{
			if (value_pos != value_end_pos)
			{
				value = msg.contents.substr(value_pos, value_end_pos - value_pos);
				value.erase(std::remove(value.begin(), value.end(), ' '), value.end());		//Strip whitespace from string
				value.erase(std::remove(value.begin(), value.end(), '\n'), value.end());
			}
		}
		report_values[key_list[i]] = value;
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
		key_list = { L"A", L"B", L"C", L"D", L"NEXTWEEKBAP", L"BAP", L"CONF", L"BD", L"SAC", L"PK", L"OL", L"NIMISSFIND", L"NIMEMREF", L"RCLA", L"LAC", L"RCT"};
	else if (type == TYPE_ENGLISH)
	{
		use_sub_id = true;
		key_list = { L"CLASSLEVEL", L"TOTALSTUDENTS", L"TOTALNONMEM", L"NEWSTUDENTS", L"NEWINV" };
	}
	else if (type == TYPE_BAPTISM_RECORD)
		key_list = { L"CONV_NAME", L"BP_DATE", L"CONF_DATE", L"WARD", L"HOME_ADDR", L"PH_NUM", L"BAP_SOURCE" };
	else if (type == TYPE_BAPTISM_SOURCE)
		key_list = { L"BAP_MISS_FIND", L"BAP_LA_REF", L"BAP_RC_REF", L"BAP_MEM_REF", L"BAP_ENGLISH", L"BAP_TOUR" };
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

	if (other.key_list.size() != this->key_list.size())
		return false;

	for (int i = 0; i < key_list.size(); i++)
	{
		if (other.key_list[i] != this->key_list[i])
			return false;
		if (other.report_values[key_list[i]] != this->report_values[key_list[i]])
			return false;
	}
	return true;
}

bool Report::operator!=(Report& other)
{
	return !((*this)==other);
}

void Report::operator+=(Report& other)
{
	for (std::map<std::wstring, std::wstring>::iterator i = other.report_values.begin(); i != other.report_values.end(); ++i)
	{
		if (this->report_values.count(i->first) > 0)	//Existing value for this report field
		{
			std::wstringstream this_ss;
			std::wstringstream other_ss;

			this_ss.str(this->report_values[i->first]);
			other_ss.str(i->second);
			int this_value;
			int other_value;
			this_ss >> this_value;
			other_ss >> other_value;

			if (!this_ss.fail() && !other_ss.fail())
			{
				int new_value = this_value + other_value;
				wchar_t intstr[8];
				_itow_s(new_value, intstr, 8, 10);
				this->report_values[i->first] = std::wstring(intstr);
			}
			else
			{
				this->report_values[i->first] = L"-";
			}
		}
		else
		{
			this->add_field(i->first, i->second);
		}
	}
}

void Report::add_field(std::wstring key, std::wstring value)
{
	report_values[key] = value;
}

void Report::remove_field(std::wstring key)
{
	report_values.erase(key);
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
	std::wstring key_name;
	std::wstring value;
	for (int i = 0; i < key_list.size(); i++)
	{
		value = L"0";
		if (stream.good())
			stream >> value;
		report_values[key_list[i]] = value;
	}

	is_new = false;

	return;
}

void Report::print(std::wostream& output)
{
	std::wstring str = get_id_str();
	if (sender_name == L"0")
		int x = 0;
	output << get_id_str();
	for (int i = 0; i < key_list.size(); i++)
	{
		std::wstring value = report_values[key_list[i]];
		output << L'\t';
		output << value;
	}
	output << std::endl;
}