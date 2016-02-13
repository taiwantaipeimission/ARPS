#include "Report.h"

#include <sstream>
#include <algorithm>

#include "Message.h"

Report::Report()
{
	set_type(TYPE_REGULAR);
}

void Report::read_message(Message msg, std::wstring date)
{
	id_str = date + L":" + msg.sender_name;
	sender_number = msg.sender_number;

	if (msg.type == Message::TYPE_REPORT)
		set_type(TYPE_REGULAR);
	else if (msg.type == Message::TYPE_REPORT_ENGLISH)
		set_type(TYPE_ENGLISH);
	else if (msg.type == Message::TYPE_REPORT_BAPTISM)
		set_type(TYPE_BAPTISM_RECORD);

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
	if (type == TYPE_REGULAR)
		key_list = { L"A", L"B", L"C", L"D", L"NEXTWEEKBAP", L"BAP", L"CONF", L"BD", L"SAC", L"PK", L"OL", L"NIMISSFIND", L"NIMEMREF", L"RCLA", L"LAC", L"RCT"};
	else if (type == TYPE_ENGLISH)
		key_list = { L"CLASSLEVEL", L"TOTALSTUDENTS", L"TOTALNONMEM", L"NEWSTUDENTS", L"NEWINV" };
	else if (type == TYPE_BAPTISM_RECORD)
		key_list = { L"CONV_NAME", L"BP_DATE", L"CONF_DATE", L"WARD", L"HOME_ADDR", L"PH_NUM", L"BAP_SOURCE" };
	else if (type == TYPE_BAPTISM_SOURCE)
		key_list = { L"BAP_MISS_FIND", L"BAP_LA_REF", L"BAP_RC_REF", L"BAP_MEM_REF", L"BAP_ENGLISH", L"BAP_TOUR" };
}

std::wstring Report::get_id_str()
{
	return id_str;
}

std::wstring Report::get_date()
{
	int last_colon_pos = id_str.find_last_of(L":");
	std::wstring date = id_str.substr(0, last_colon_pos);
	return date;
}

std::wstring Report::get_sender_name()
{
	int name_begin_pos = id_str.find_last_of(L":") + 1;
	std::wstring name = id_str.substr(name_begin_pos, id_str.length() - name_begin_pos);
	return name;
}

bool Report::operator==(Report& other)
{
	if (other.id_str != this->id_str)
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
	stream >> id_str;
	stream >> sender_number;

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
	output << id_str << L"\t" << sender_number;
	for (int i = 0; i < key_list.size(); i++)
	{
		std::wstring value = report_values[key_list[i]];
		output << L'\t';
		output << value;
	}
	output << std::endl;
}